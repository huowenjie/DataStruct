#include <stdlib.h>
#include <string.h>

#include "rbtree.h"
#include "../stack/stack.h"

#define RBTREE_COLOR_RED 0      /* 定义红色 */
#define RBTREE_COLOR_BLACK 1    /* 定义黑色 */

/* 红黑树结点 */
struct rb_node_st
{
    int color;
    int bh;

    RB_NODE *parent;
    RB_NODE *left;
    RB_NODE *right;

    const char *key;
    void *data;
};

/* 红黑树 */
struct rb_tree_st
{
    RB_NODE *root;

    int count;
};

/* 对结点 node 进行左旋转 */
static int rb_left_rotate(RB_TREE *tree, RB_NODE *node);

/* 对结点 node 进行右旋转 */
static int rb_right_rotate(RB_TREE *tree, RB_NODE *node);

/* 插入结点时调整红黑树 */
static int rb_insert_fixup_tree(RB_TREE *tree, RB_NODE *node);

/* 删除节点时调整红黑树，parent 是为了帮助检查 node 是否是哨兵节点 */
static int rb_delete_fixup_tree(RB_TREE *tree, RB_NODE *parent, RB_NODE *node);

/* 节点移动 */
static int rb_node_transplant(RB_TREE *tree, RB_NODE *dest, RB_NODE *src);

/**
 * 声明栈方法
 * 
 * RB_STACK
 * extern int rb_stack_push(RB_STACK *stack, RB_NODE *node);
 * extern int rb_stack_pop(RB_STACK *stack, RB_NODE **ret);
 */
STACK_DEFINE(rb, RB, RB_NODE *);

/*-------------------------------------------------------*/

RB_TREE *rb_create()
{
    RB_TREE *tree = malloc(sizeof(RB_TREE));
    memset(tree, 0, sizeof(sizeof(RB_TREE)));

    tree->root = NULL;
    tree->count = 0;

    return tree;
}

void rb_destroy(RB_TREE *tree)
{
    if (tree) {
        RB_STACK stack;
        RB_STACK tmp;

        RB_NODE **buf = NULL;
        RB_NODE *node = NULL;

        int count = tree->count;
        if (count <= 0) {
            return;
        }

        /* 后序遍历需要的栈空间是先序和中序的两倍 */
        buf = malloc(2 * count * sizeof(RB_NODE *));
        memset(buf, 0, 2 * count * sizeof(RB_NODE *));

        stack.elems = buf;
        stack.num = 0;
        stack.size = count;

        tmp.elems = buf + count;
        tmp.num = 0;
        tmp.size = count;

        node = tree->root;

        while (node || tmp.num > 0) {
            /* 一直访问右子树 */
            while (node) {
                /* 将访问到的节点临时保存在 stack 中 */
                rb_stack_push(&stack, node);
                rb_stack_push(&tmp, node);
                node = node->right;
            }

            if (tmp.num > 0) {
                /* 弹出压入的结点 */
                rb_stack_pop(&tmp, &node);

                /* 访问左子树 */
                node = node->left;
            }
        }

        /* 依次弹出节点并释放 */
        while (stack.num > 0) {
            rb_stack_pop(&stack, &node);
            free(node);
        }

        free(buf);
        free(tree);
    }
}

int rb_insert(RB_TREE *tree, const char *key, void *data)
{
    RB_NODE *node = NULL;
    RB_NODE *target = NULL;
    RB_NODE *add = NULL;

    int cmp = 0;

    if (!tree) {
        return -1;
    }

    if (!key || !*key) {
        return -1;
    }

    node = tree->root;

    /* 搜索红黑树并找到合适的插入位置 */
    while (node) {
        target = node;
        cmp = strcmp(key, (const char *)node->key);

        if (!cmp) {
            /* 结点已存在 */
            return -1;
        }

        if (cmp < 0) {
            /* 搜索左子树 */
            node = node->left;
        } else {
            /* 搜索右子树 */
            node = node->right;
        }
    }

    /* 为插入的结点申请内存 */
    add = malloc(sizeof(RB_NODE));
    memset(add, 0, sizeof(RB_NODE));

    add->color = RBTREE_COLOR_RED;
    add->parent = NULL;
    add->left = NULL;
    add->right = NULL;
    add->key = (void *)key;
    add->data = data;
    add->bh = 1;

    if (!target) {
        /* 如果是根结点直接插入 */
        tree->root = add;
    } else if (cmp < 0) {
        target->left = add;
    } else {
        target->right = add;
    }

    add->parent = target;

    /**
     * 1.插入一个红色结点，有可能导致红黑树的性质2或者性质4发生变化，所以需要
     * 调整；
     * 2.如果 add 是根结点，那么性质2会发生变化；
     * 3.如果 add 的父结点是红色的，那么性质4会发生变化；
     * 4.如果 add 的父结点是黑色的，那么保持不变即可；
     */
    if (rb_insert_fixup_tree(tree, add)) {
        return -1;
    }

    tree->count++;
    return 0;
}

int rb_delete(RB_TREE *tree, const char *key, void **data)
{
    RB_NODE *target = NULL;
    RB_NODE *tmp = NULL;
    RB_NODE *parent = NULL;

    int cmp = 0;
    int color = RBTREE_COLOR_RED;

    if (!tree) {
        return -1;
    }

    if (!key || !*key) {
        return -1;
    }

    target = tree->root;

    /* 搜索红黑树并找到待移除节点 */
    while (target) {
        cmp = strcmp(key, (const char *)target->key);

        if (!cmp) {
            /* 结点存在 */
            break;
        }

        if (cmp < 0) {
            /* 搜索左子树 */
            target = target->left;
        } else {
            /* 搜索右子树 */
            target = target->right;
        }
    }

    if (!target) {
        return -1;
    }

    /* 记录初始颜色 */
    color = target->color;

    if (!target->left) {
        tmp = target->right;
        parent = target->parent;

        rb_node_transplant(tree, target, tmp);
    } else if (!target->right) {
        tmp = target->left;
        parent = target->parent;

        rb_node_transplant(tree, target, tmp);
    } else {
        /**
         * 如果待删节点的左子树和右子树均不为 NULL，则找到被删除节点的后继，也就是它右子树的最
         * 小值和目标节点交换，目标节点的父结点、右子树、左子树分别变成了替换节点的父结点、右
         * 子树、左子树。
         */

        /* 查找 target 后继 */
        RB_NODE *mini = target->right;

        while (mini->left) {
            mini = mini->left;
        }

        color = mini->color;

        /* mini 已经是待删节点的后继，说明它没有左子树，所以直接用它的右子树替换掉它 */
        tmp = mini->right;
        parent = mini->parent;

        /* 用后继的右子树替换后继 */
        rb_node_transplant(tree, mini, tmp);

        /* 用后继替换待删除节点 */
        rb_node_transplant(tree, target, mini);

        mini->left = target->left;
        if (target->left) {
            mini->left->parent = mini;
        }

        mini->right = target->right;
        if (mini->right) {
            mini->right->parent = mini;
        }

        /* 这里需要替换颜色 */
        mini->color = target->color;
        mini->bh = target->bh;
    }

    /* 被删节点为黑色，可能会影响红黑树的性质，所以需要重新调整至平衡 */
    if (color == RBTREE_COLOR_BLACK) {
        /* 这个 tmp 指的是删除目标节点后，移动到原目标节点后继位置上的节点 */
        rb_delete_fixup_tree(tree, parent, tmp);
    }

    tree->count--;

    if (data) {
        *data = target->data;
    }

    free(target);
    return 0;
}

int rb_find(RB_TREE *tree, const char *key, void **data)
{
    RB_NODE *target = NULL;
    int cmp = 0;

    if (!tree) {
        return -1;
    }

    if (!key || !*key || !data) {
        return -1;
    }

    target = tree->root;

    /* 搜索红黑树并找到节点 */
    while (target) {
        cmp = strcmp(key, (const char *)target->key);

        if (!cmp) {
            /* 结点存在 */
            break;
        }

        if (cmp < 0) {
            /* 搜索左子树 */
            target = target->left;
        } else {
            /* 搜索右子树 */
            target = target->right;
        }
    }

    if (!target) {
        return -1;
    }

    *data = target->data;
    return 0;
}

int rb_iterate(RB_TREE *tree, void (*visit_before)(void *key, void *data, int bh))
{    
    RB_STACK stack;
    RB_NODE **nodes = NULL;
    RB_NODE *node = NULL;
    int count = 0;

    if (!tree || !visit_before) {
        return -1;
    }

    count = tree->count;
    if (count <= 0) {
        return -1;
    }

    nodes = malloc(count * sizeof(RB_NODE *));
    memset(nodes, 0, count * sizeof(RB_NODE *));

    stack.elems = nodes;
    stack.num = 0;
    stack.size = count;

    node = tree->root;

    while (node || stack.num > 0) {
        /* 遇到一个节点直接压栈，然后访问左子树 */
        while (node) {
            rb_stack_push(&stack, node);
            node = node->left;
        }

        /* 左子树没有左儿子时，访问该节点的右儿子的左子树，以此类推 */
        if (stack.num > 0) {
            /* 弹出压入的结点 */
            rb_stack_pop(&stack, &node);

            visit_before((void *)node->key, node->data, node->bh);

            /* 访问节点又子树 */
            node = node->right;
        }
    }

    free(nodes);
    return 0;
}

/*-------------------------------------------------------*/

int rb_left_rotate(RB_TREE *tree, RB_NODE *node)
{
    RB_NODE *y = NULL;

    if (!tree || !node) {
        return -1;
    }

    if (tree->count <= 0) {
        return -1;
    }

    y = node->right;
    if (!y) {
        return -1;
    }

    node->right = y->left;
    if (y->left) {
        y->left->parent = node;
    }
    
    y->left = node;
    y->parent = node->parent;

    /* 处理各种父结点的边界情况 */
    if (node == tree->root) {
        tree->root = y;
    } else if (node == node->parent->left) {
        node->parent->left = y;
    } else if (node == node->parent->right) {
        node->parent->right = y;
    }

    node->parent = y;
    return 0;
}

int rb_right_rotate(RB_TREE *tree, RB_NODE *node)
{
    RB_NODE *x = NULL;

    if (!tree || !node) {
        return -1;
    }

    if (tree->count <= 0) {
        return -1;
    }

    x = node->left;
    if (!x) {
        return -1;
    }

    node->left = x->right;
    if (x->right) {
        x->right->parent = node;
    }

    x->right = node;
    x->parent = node->parent;

    /* 处理各种父结点的边界情况 */
    if (node == tree->root) {
        tree->root = x;
    } else if (node == node->parent->left) {
        node->parent->left = x;
    } else if (node == node->parent->right) {
        node->parent->right = x;
    }

    node->parent = x;
    return 0;
}

int rb_insert_fixup_tree(RB_TREE *tree, RB_NODE *node)
{
    RB_NODE *tmp = NULL;

    if (!tree || !node) {
        return -1;
    }

    tmp = node;

    /**
     * 1.当前结点的父结点是红色时才需要调整；
     * 2.如果 node 是根结点，插入结点后红黑树一定违反性质2（因为它是红色结点），
     *   所以我们在本函数结尾处会将根结点的颜色染成黑色以确保性质2正确；
     * 3.如果当前结点的父亲结点存在且其为红色，那么它的祖父结点必然存在，因为父亲
     *   结点若是红色则父亲结点就不可能是根结点，所以父亲结点必然存在它的父亲结点
     *   也就是当前我们新插入结点的祖父结点，所以我们在循环中可以 tmp->parent->parent 
     *   来引用其祖父结点，不必担心空指针。
     */
    while (tmp->parent && tmp->parent->color == RBTREE_COLOR_RED) {
        RB_NODE *uncle = NULL;
        RB_NODE *parent = tmp->parent;

        /**
         * 我们局部来看，我们将当前新增的结点称为儿子结点：
         *     由于红黑树性质 4 “如果一个结点是红色的，则它两个子结点都是黑色”，儿
         * 子结点 tmp 和它的父结点、叔叔结点都是红色，为了满足性质 4，必须将父亲结点
         * 和儿子结点变为不同的颜色；但是，如果单单将父亲结点或者当前结点其中一个
         * 变为黑色后，那么祖父结点的两个儿子的黑高就不同了，这又违反了性质5，所以最
         * 好的方案是同时改变祖父的左儿子（父亲）和右儿子（叔叔）以及祖父的颜色，
         * 改变了祖父的颜色，然后将祖父当成儿子，重新向上循环这个过程；
         * 
         *     第二种情况和第三种情况可以合在一起讨论，如果叔叔的颜色和父亲不一样是黑
         * 色的，同时儿子结点是父亲结点左儿子或者右儿子，可以通过两次旋转来调整结点。
         */

        /* 判断当前结点的父亲结点是否是祖父的左孩子或者右孩子 */
        if (parent == parent->parent->left) {
            uncle = parent->parent->right;

            if (uncle && uncle->color == RBTREE_COLOR_RED) {
                uncle->color = RBTREE_COLOR_BLACK;
                parent->color = RBTREE_COLOR_BLACK;
                parent->parent->color = RBTREE_COLOR_RED;

                parent->parent->bh++;
                tmp = parent->parent;
                parent = tmp->parent;
            } else if (tmp == parent->right) {
                tmp = parent;
                rb_left_rotate(tree, tmp);
                parent = tmp->parent;

                parent->color = RBTREE_COLOR_BLACK;
                parent->parent->color = RBTREE_COLOR_RED;
                rb_right_rotate(tree, parent->parent);
            } else if (tmp == parent->left) {
                parent->color = RBTREE_COLOR_BLACK;
                parent->parent->color = RBTREE_COLOR_RED;
                rb_right_rotate(tree, parent->parent);
            }
        } else {
            uncle = parent->parent->left;

            if (uncle && uncle->color == RBTREE_COLOR_RED) {
                uncle->color = RBTREE_COLOR_BLACK;
                parent->color = RBTREE_COLOR_BLACK;
                parent->parent->color = RBTREE_COLOR_RED;

                parent->parent->bh++;
                tmp = parent->parent;
                parent = tmp->parent;
            } else if (tmp == parent->left) {
                tmp = parent;
                rb_right_rotate(tree, tmp);
                parent = tmp->parent;

                parent->color = RBTREE_COLOR_BLACK;
                parent->parent->color = RBTREE_COLOR_RED;
                rb_left_rotate(tree, parent->parent);
            } else if (tmp == parent->right) {
                parent->color = RBTREE_COLOR_BLACK;
                parent->parent->color = RBTREE_COLOR_RED;
                rb_left_rotate(tree, parent->parent);
            }
        }
    }

    tree->root->color = RBTREE_COLOR_BLACK;
    return 0;
}

int rb_delete_fixup_tree(RB_TREE *tree, RB_NODE *parent, RB_NODE *node)
{
    if (!tree) {
        return -1;
    }

    /**
     * 1.!node 表明节点是一个哨兵节点，它必然是黑色的；
     * 2.parent 存在说明 node 不为根节点;
     * 3.节点 node 临时被视为双重黑色或者红黑色。
     */
    while (parent && (!node || node->color == RBTREE_COLOR_BLACK)) {
        /* 判断当前节点是 parent 的左儿子还是右儿子 */
        if (node == parent->left) {
            /* 获取兄弟节点 */
            RB_NODE *brother = parent->right;

            if (brother->color == RBTREE_COLOR_RED) {
                brother->color = RBTREE_COLOR_BLACK;
                parent->color = RBTREE_COLOR_RED;

                /* 提前设置 node 新的兄弟节点 */
                brother = brother->left;

                rb_left_rotate(tree, parent);
            }

            if (!brother) {
                break;
            }

            if ((!brother->left || brother->left->color == RBTREE_COLOR_BLACK) &&
                (!brother->right || brother->right->color == RBTREE_COLOR_BLACK)) {
                brother->color = RBTREE_COLOR_RED;
                parent->bh--;

                node = !node ? parent : node->parent;
                parent = parent->parent;
            } else if (!brother->right || brother->right->color == RBTREE_COLOR_BLACK) {
                if (brother->left) {
                    brother->left->color = RBTREE_COLOR_BLACK;
                }

                brother->color = RBTREE_COLOR_RED;
                rb_right_rotate(tree, brother);
                brother = parent->right;

                brother->color = parent->color;
                parent->color = RBTREE_COLOR_BLACK;

                if (brother->right) {
                    brother->right->color = RBTREE_COLOR_BLACK;
                }

                rb_left_rotate(tree, parent);
                brother = tree->root;
            } else {
                if (parent->color == RBTREE_COLOR_RED) {
                    parent->bh--;
                    brother->bh++;
                }

                brother->color = parent->color;
                parent->color = RBTREE_COLOR_BLACK;

                if (brother->right) {
                    brother->right->color = RBTREE_COLOR_BLACK;
                }

                rb_left_rotate(tree, parent);
                node = tree->root;
                parent = NULL;
            }
        } else {
            RB_NODE *brother = parent->left;

            if (brother->color == RBTREE_COLOR_RED) {
                brother->color = RBTREE_COLOR_BLACK;
                parent->color = RBTREE_COLOR_RED;

                /* 提前设置 node 新的兄弟节点 */
                brother = brother->right;

                rb_right_rotate(tree, parent);
            }

            if (!brother) {
                break;
            }

            if ((!brother->left || brother->left->color == RBTREE_COLOR_BLACK) &&
                (!brother->right || brother->right->color == RBTREE_COLOR_BLACK)) {
                brother->color = RBTREE_COLOR_RED;
                parent->bh--;

                node = !node ? parent : node->parent;
                parent = parent->parent;
            } else if (!brother->left || brother->left->color == RBTREE_COLOR_BLACK) {
                if (brother->right) {
                    brother->right->color = RBTREE_COLOR_BLACK;
                }

                brother->color = RBTREE_COLOR_RED;
                rb_left_rotate(tree, brother);
                brother = parent->left;

                brother->color = parent->color;
                parent->color = RBTREE_COLOR_BLACK;

                if (brother->left) {
                    brother->left->color = RBTREE_COLOR_BLACK;
                }

                rb_right_rotate(tree, parent);
                brother = tree->root;
            } else {
                if (parent->color == RBTREE_COLOR_RED) {
                    parent->bh--;
                    brother->bh++;
                }

                brother->color = parent->color;
                parent->color = RBTREE_COLOR_BLACK;

                if (brother->left) {
                    brother->left->color = RBTREE_COLOR_BLACK;
                }

                rb_right_rotate(tree, parent);
                node = tree->root;
                parent = NULL;
            }
        }
    }

    if (node) {
        node->color = RBTREE_COLOR_BLACK;
    }
    return 0;
}

int rb_node_transplant(RB_TREE *tree, RB_NODE *dest, RB_NODE *src)
{
    if (!tree || !dest) {
        return -1;
    }

    if (dest == tree->root) {
        tree->root = src;
    } else if (dest->parent->left == dest) {
        dest->parent->left = src;
    } else if (dest->parent->right == dest) {
        dest->parent->right = src;
    }

    if (src) {
        src->parent = dest->parent;
    }
    return 0;
}

/*-------------------------------------------------------*/

STACK_IMPLEMENT(rb, RB, RB_NODE *);
