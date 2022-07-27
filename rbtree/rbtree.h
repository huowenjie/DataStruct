#ifndef __RBTREE_H__
#define __RBTREE_H__

/**
 * 红黑树在每个结点上增加一个存储单元来表示节点的颜色，可以是 RED 或者 BLACK,
 * 通过对任何一条从根到叶子的简单路径上各个结点的颜色进行约束，红黑树确保没有
 * 一条路径会比其他路径长出两倍，因而近似平衡。
 * 
 * 一颗红黑树首先是一颗二叉树，同时满足以下性质：
 * 1.每个结点是红色的或者是黑色的；
 * 2.根节点是黑色的；
 * 3.每个叶子结点是黑色的；
 * 4.如果一个结点是红色的，则它的两个子结点都是黑色的；
 * 5.对于每个结点，从该结点到其所有后代的叶子结点的简单路径上，均包含相同数目的黑色结点。
 * 
 * 黑高（bh）：
 *     从某个结点 x 出发（不包含 x）到达一个叶子结点的任意一条简单路径上的黑色结点个数称为
 * 该结点的黑高，记为 bh(x)。红黑树的黑高即为其根节点的黑高。
 * 
 */

typedef struct rb_node_st RB_NODE;
typedef struct rb_tree_st RB_TREE;

/* 创建一颗红黑树 */
RB_TREE *rb_create();

/* 销毁红黑树 */
void rb_destroy(RB_TREE *tree);

/* 插入数据 */
int rb_insert(RB_TREE *tree, const char *key, void *data);

/* 删除数据，data 不为 NULL，返回移除的数据地址 */
int rb_delete(RB_TREE *tree, const char *key, void **data);

/* 查找数据 */
int rb_find(RB_TREE *tree, const char *key, void **data);

/* 遍历红黑树 */
int rb_iterate(RB_TREE *tree, void (*visit_before)(void *key, void *data, int bh));

#endif /* __RBTREE_H__ */
