#include <stdlib.h>
#include <string.h>

#include "rbtree.h"
#include "../stack/stack.h"

/*===========================================================================*/

#define RBTREE_COLOR_RED   0x00000000      /* 定义红色 */
#define RBTREE_COLOR_BLACK 0x00000001      /* 定义黑色 */
#define RBTREE_COLOR_MASK  0x0000FFFF      /* 颜色掩码 */
#define RBTREE_NIL_NODE    0x00010000      /* 哨兵节点 */

/* 红黑树结点 */
struct rb_node_st
{
    unsigned int flag;

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
    RB_NODE *nil;

    int count;
};

/* 对结点 node 进行左旋转 */
static int rb_left_rotate(RB_TREE *tree, RB_NODE *node);

/* 对结点 node 进行右旋转 */
static int rb_right_rotate(RB_TREE *tree, RB_NODE *node);

/* 插入结点时调整红黑树 */
static int rb_insert_fixup_tree(RB_TREE *tree, RB_NODE *node);

/* 删除节点时调整红黑树 */
static int rb_delete_fixup_tree(RB_TREE *tree, RB_NODE *node);

/* 节点移动 */
static int rb_node_transplant(RB_TREE *tree, RB_NODE *dest, RB_NODE *src);

/* 设置颜色 */
static void rb_node_set_color(RB_NODE *node, unsigned int color);

/* 获取颜色 */
static unsigned int rb_node_get_color(RB_NODE *node);

/* 是否为哨兵节点，如果哨兵节点或者空节点，返回 1，否则返回 0 */
static int rb_node_is_nil(RB_NODE *node);

/**
 * 声明栈方法
 * 
 * RB_STACK
 * extern int rb_stack_push(RB_STACK *stack, RB_NODE *node);
 * extern int rb_stack_pop(RB_STACK *stack, RB_NODE **ret);
 */
STACK_DEFINE(rb, RB, RB_NODE *);

/*===========================================================================*/

RB_TREE *rb_create()
{
    RB_NODE *nil = NULL;
    RB_TREE *tree = malloc(sizeof(RB_TREE));
    memset(tree, 0, sizeof(sizeof(RB_TREE)));

    nil = malloc(sizeof(RB_NODE));
    memset(nil, 0, sizeof(RB_NODE));
    nil->flag = RBTREE_COLOR_BLACK | RBTREE_NIL_NODE;

    tree->root = NULL;
    tree->count = 0;
    tree->nil = nil;

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

        while ((node && !rb_node_is_nil(node)) || tmp.num > 0) {
            while (node && !rb_node_is_nil(node)) {
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
        free(tree->nil);
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
    while (node && !rb_node_is_nil(node)) {
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

    add->flag = RBTREE_COLOR_RED;
    add->parent = NULL;
    add->left = tree->nil;
    add->right = tree->nil;
    add->key = (void *)key;
    add->data = data;

    if (!target) {
        tree->root = add;
    } else if (cmp < 0) {
        target->left = add;
    } else {
        target->right = add;
    }

    add->parent = target;

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

    int cmp = 0;
    unsigned int color = RBTREE_COLOR_RED;

    if (!tree) {
        return -1;
    }

    if (!key || !*key) {
        return -1;
    }

    target = tree->root;

    /* 搜索红黑树并找到待移除节点 */
    while (target && !rb_node_is_nil(target)) {
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

    if (!target || rb_node_is_nil(target)) {
        return -1;
    }

    /* 记录初始颜色 */
    color = rb_node_get_color(target);

    if (rb_node_is_nil(target->left)) {
        tmp = target->right;
        rb_node_transplant(tree, target, tmp);
    } else if (rb_node_is_nil(target->right)) {
        tmp = target->left;
        rb_node_transplant(tree, target, tmp);
    } else {
        /**
         * 如果待删节点的左子树和右子树均不为 NULL，则找到被删除节点的后继，也就是它右子树的最
         * 小值和目标节点交换，目标节点的父结点、右子树、左子树分别变成了替换节点的父结点、右
         * 子树、左子树。
         */

        /* 查找 target 后继 */
        RB_NODE *mini = target->right;

        while (!rb_node_is_nil(mini->left)) {
            mini = mini->left;
        }

        color = rb_node_get_color(mini);

        /* mini 已经是待删节点的后继，说明它没有左子树，所以直接用它的右子树替换掉它 */
        tmp = mini->right;

        if (mini->parent == target) {
            tmp->parent = mini;
        } else {
            /* 用后继的右子树替换后继 */
            rb_node_transplant(tree, mini, tmp);
            mini->right = target->right;
            mini->right->parent = mini;
        }

        /* 用后继替换待删除节点 */
        rb_node_transplant(tree, target, mini);

        mini->left = target->left;
        mini->left->parent = mini;
        rb_node_set_color(mini, target->flag & RBTREE_COLOR_MASK);
    }

    /* 被删节点为黑色，可能会影响红黑树的性质，所以需要重新调整至平衡 */
    if (color == RBTREE_COLOR_BLACK) {
        rb_delete_fixup_tree(tree, tmp);
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
    while (target && !rb_node_is_nil(target)) {
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

    if (!target || rb_node_is_nil(target)) {
        return -1;
    }

    *data = target->data;
    return 0;
}

int rb_iterate(
    RB_TREE *tree, void (*visit_before)(void *key, void *data, void *args), void *args)
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

    while ((node && !rb_node_is_nil(node)) || stack.num > 0) {
        /* 遇到一个节点直接压栈，然后访问左子树 */
        while (node && !rb_node_is_nil(node)) {
            rb_stack_push(&stack, node);
            node = node->left;
        }

        /* 左子树没有左儿子时，访问该节点的右儿子的左子树，以此类推 */
        if (stack.num > 0) {
            /* 弹出压入的结点 */
            rb_stack_pop(&stack, &node);
            visit_before((void *)node->key, node->data, args);

            /* 访问节点右子树 */
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

    y = node->right;
    node->right = y->left;

    if (!rb_node_is_nil(y->left)) {
        y->left->parent = node;
    }

    y->parent = node->parent;

    /* 处理各种父结点的边界情况 */
    if (node == tree->root) {
        tree->root = y;
    } else if (node == node->parent->left) {
        node->parent->left = y;
    } else if (node == node->parent->right) {
        node->parent->right = y;
    }

    y->left = node;
    node->parent = y;
    return 0;
}

int rb_right_rotate(RB_TREE *tree, RB_NODE *node)
{
    RB_NODE *x = NULL;

    if (!tree || !node) {
        return -1;
    }

    x = node->left;
    node->left = x->right;

    if (!rb_node_is_nil(x->right)) {
        x->right->parent = node;
    }

    x->parent = node->parent;

    /* 处理各种父结点的边界情况 */
    if (node == tree->root) {
        tree->root = x;
    } else if (node == node->parent->left) {
        node->parent->left = x;
    } else if (node == node->parent->right) {
        node->parent->right = x;
    }

    x->right = node;
    node->parent = x;
    return 0;
}

int rb_insert_fixup_tree(RB_TREE *tree, RB_NODE *node)
{
    RB_NODE *tmp = NULL;

    if (!tree || !node || rb_node_is_nil(node)) {
        return -1;
    }

    tmp = node;

    while (tmp->parent && (rb_node_get_color(tmp->parent) == RBTREE_COLOR_RED)) {
        RB_NODE *uncle = tree->nil;
        RB_NODE *parent = tmp->parent;

        /* 判断当前结点的父亲结点是否是祖父的左孩子或者右孩子 */
        if (parent == parent->parent->left) {
            uncle = parent->parent->right;

            if (uncle && (rb_node_get_color(uncle) == RBTREE_COLOR_RED)) {
                rb_node_set_color(uncle, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent->parent, RBTREE_COLOR_RED);

                tmp = parent->parent;
                parent = tmp->parent;
            } else if (tmp == parent->right) {
                tmp = parent;
                rb_left_rotate(tree, tmp);
                parent = tmp->parent;

                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent->parent, RBTREE_COLOR_RED);
                rb_right_rotate(tree, parent->parent);
            } else if (tmp == parent->left) {
                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent->parent, RBTREE_COLOR_RED);
                rb_right_rotate(tree, parent->parent);
            }
        } else {
            uncle = parent->parent->left;

            if (uncle && (rb_node_get_color(uncle) == RBTREE_COLOR_RED)) {
                rb_node_set_color(uncle, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent->parent, RBTREE_COLOR_RED);

                tmp = parent->parent;
                parent = tmp->parent;
            } else if (tmp == parent->left) {
                tmp = parent;
                rb_right_rotate(tree, tmp);
                parent = tmp->parent;

                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent->parent, RBTREE_COLOR_RED);
                rb_left_rotate(tree, parent->parent);
            } else if (tmp == parent->right) {
                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent->parent, RBTREE_COLOR_RED);
                rb_left_rotate(tree, parent->parent);
            }
        }
    }

    rb_node_set_color(tree->root, RBTREE_COLOR_BLACK);
    return 0;
}

int rb_delete_fixup_tree(RB_TREE *tree, RB_NODE *node)
{
    if (!tree) {
        return -1;
    }

    while (node != tree->root && (rb_node_get_color(node) == RBTREE_COLOR_BLACK)) {
        RB_NODE *parent = node->parent;

        if (node == parent->left) {
            RB_NODE *brother = parent->right;

            if (rb_node_get_color(brother) == RBTREE_COLOR_RED) {
                rb_node_set_color(brother, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent, RBTREE_COLOR_RED);

                rb_left_rotate(tree, parent);
                parent = node->parent;
                brother = parent->right;
            }

            if ((rb_node_get_color(brother->left) == RBTREE_COLOR_BLACK) &&
                (rb_node_get_color(brother->right) == RBTREE_COLOR_BLACK)) {
                rb_node_set_color(brother, RBTREE_COLOR_RED);
                node = parent;
            } else if (rb_node_get_color(brother->right) == RBTREE_COLOR_BLACK) {
                rb_node_set_color(brother->left, RBTREE_COLOR_BLACK);
                rb_node_set_color(brother, RBTREE_COLOR_RED);

                rb_right_rotate(tree, brother);
                parent = node->parent;
                brother = parent->right;

                rb_node_set_color(brother, parent->flag & RBTREE_COLOR_MASK);
                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(brother->right, RBTREE_COLOR_BLACK);

                rb_left_rotate(tree, parent);
                node = tree->root;
            } else {
                rb_node_set_color(brother, parent->flag & RBTREE_COLOR_MASK);
                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(brother->right, RBTREE_COLOR_BLACK);

                rb_left_rotate(tree, parent);
                node = tree->root;
            }
        } else {
            RB_NODE *brother = parent->left;

            if (rb_node_get_color(brother) == RBTREE_COLOR_RED) {
                rb_node_set_color(brother, RBTREE_COLOR_BLACK);
                rb_node_set_color(parent, RBTREE_COLOR_RED);

                rb_right_rotate(tree, parent);
                parent = node->parent;
                brother = parent->left;
            }

            if ((rb_node_get_color(brother->right) == RBTREE_COLOR_BLACK) &&
                (rb_node_get_color(brother->left) == RBTREE_COLOR_BLACK)) {
                rb_node_set_color(brother, RBTREE_COLOR_RED);
                node = parent;
            } else if (rb_node_get_color(brother->left) == RBTREE_COLOR_BLACK) {
                rb_node_set_color(brother->right, RBTREE_COLOR_BLACK);
                rb_node_set_color(brother, RBTREE_COLOR_RED);

                rb_left_rotate(tree, brother);
                parent = node->parent;
                brother = parent->left;

                rb_node_set_color(brother, parent->flag & RBTREE_COLOR_MASK);
                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(brother->left, RBTREE_COLOR_BLACK);

                rb_right_rotate(tree, parent);
                node = tree->root;
            } else {
                rb_node_set_color(brother, parent->flag & RBTREE_COLOR_MASK);
                rb_node_set_color(parent, RBTREE_COLOR_BLACK);
                rb_node_set_color(brother->left, RBTREE_COLOR_BLACK);

                rb_right_rotate(tree, parent);
                node = tree->root;
            }
        }
    }

    rb_node_set_color(node, RBTREE_COLOR_BLACK);
    return 0;
}

int rb_node_transplant(RB_TREE *tree, RB_NODE *dest, RB_NODE *src)
{
    if (!tree || !dest || !src) {
        return -1;
    }

    if (!dest->parent) {
        tree->root = src;
    } else if (dest->parent->left == dest) {
        dest->parent->left = src;
    } else if (dest->parent->right == dest) {
        dest->parent->right = src;
    }

    src->parent = dest->parent;
    return 0;
}

void rb_node_set_color(RB_NODE *node, unsigned int color)
{
    if (node) {
        node->flag = node->flag & (~RBTREE_COLOR_MASK);
        node->flag |= color;
    }
}

unsigned int rb_node_get_color(RB_NODE *node)
{
    if (node) {
        return node->flag & RBTREE_COLOR_MASK;
    }
    return RBTREE_COLOR_BLACK;
}

int rb_node_is_nil(RB_NODE *node)
{
    if (!node || (node->flag & RBTREE_NIL_NODE)) {
        return 1;
    }
    return 0;
}

/*===========================================================================*/

STACK_IMPLEMENT(rb, RB, RB_NODE *);

/*===========================================================================*/
