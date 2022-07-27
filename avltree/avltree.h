#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__

/*===========================================================================*/
/* AVL 树 */
/*===========================================================================*/

/* 树节点 */
typedef struct tree_node_st TREE_NODE;

struct tree_node_st
{
	/* 节点高度 */
	int height;

	TREE_NODE *left;
	TREE_NODE *right;

	/* 数据 */
	char data;
};

/* 定义一颗平衡树 */
typedef struct avl_tree_st
{
	int count;
	TREE_NODE *root;
} AVL_TREE;

/* 创建/销毁 */
AVL_TREE *avl_tree_create();
void avl_tree_destroy(AVL_TREE *tree);

/* 清空节点 */
void avl_tree_clear(AVL_TREE *tree);

/* 插入 */
int avl_tree_insert(AVL_TREE *tree, char data);

/* 删除 */
int avl_tree_delete(AVL_TREE *tree, char data);

/*===========================================================================*/

#endif /* __AVL_TREE_H__ */
