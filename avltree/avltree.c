#include <stdlib.h>
#include "avltree.h"

/*===========================================================================*/

#define TREE_HEIGHT(t) ((t)->height)
#define HEIGHT_MAX(h1, h2) (((h1) > (h2)) ? (h1) : (h2))

/* 获取节点高度 */
static int avl_node_height(TREE_NODE *node);
static int avl_node_max_height(TREE_NODE *l, TREE_NODE *r);

/* 右旋转 */
static int avl_tree_right_rotate(TREE_NODE **target);

/* 左旋转 */
static int avl_tree_left_rotate(TREE_NODE **target);

/* 左-右 双旋转 */
static int avl_tree_lr_rotate(TREE_NODE **target);

/* 右-左 双旋转 */
static int avl_tree_rl_rotate(TREE_NODE **target);

/* 对比左子树和右子树的高度 */
static int avl_tree_comp_height(TREE_NODE *left, TREE_NODE *right);

/* 递归插入 */
static int avl_tree_insert_node(TREE_NODE **node, char data);

/* 递归删除 */
static int avl_tree_delete_node(TREE_NODE **node, char data);

/* 递归清空节点 */
static void avl_tree_clear_nodes(TREE_NODE *node);

/* 移除目标节点，同时交换当前节点和到被删除节点目标位置 */
static int avl_tree_del_exchange(TREE_NODE **node, TREE_NODE *target);

/* 平衡节点 */
static int avl_tree_rebalance(TREE_NODE **node);

/*===========================================================================*/

AVL_TREE *avl_tree_create()
{
	AVL_TREE *tree = malloc(sizeof(AVL_TREE));

	if (tree) {
		tree->count = 0;
		tree->root = NULL;
	}
	return tree;
}

void avl_tree_destroy(AVL_TREE *tree)
{
	if (tree) {
		free(tree);
	}
}

void avl_tree_clear(AVL_TREE *tree)
{
	if (tree) {
		avl_tree_clear_nodes(tree->root);

		tree->root  = NULL;
		tree->count = 0;
	}
}

int avl_tree_insert(AVL_TREE *tree, char data)
{
	if (tree) {
		if (!avl_tree_insert_node(&tree->root, data)) {
			tree->count++;
			return 0;
		}
	}
	return -1;
}

int avl_tree_delete(AVL_TREE *tree, char data)
{
	if (tree) {
		if (!avl_tree_delete_node(&tree->root, data)) {
			tree->count--;
			return 0;
		}
	}
	return -1;
}

/*===========================================================================*/

int avl_node_height(TREE_NODE *node)
{
	if (node) {
		return node->height;
	}
	return -1;
}

int avl_node_max_height(TREE_NODE *l, TREE_NODE *r)
{
	int lh = -1;
	int rh = -1;

	if (l) {
		lh = l->height;
	}

	if (r) {
		rh = r->height;
	}

	return HEIGHT_MAX(lh, rh);
}

int avl_tree_right_rotate(TREE_NODE **target)
{
	TREE_NODE *root = NULL;
	TREE_NODE *left = NULL;
	TREE_NODE *right = NULL;

	/* 空节点不可旋转 */
	if (!target || !*target) {
		return -1;
	}

	root = *target;
	left = root->left;

	/* 右旋转的话，左节点不可能没有值 */
	if (!left) {
		return -1;
	}

	/*
	 * 1.目标节点变为左子树的右子节点
	 * 2.左子树的右节点变为目标节点的左子节点
	 * 3.更新节点深度信息
	 * 4.改变根节点为目标节点的左子节点
	 */

	right = left->right;
	left->right = root;
	root->left  = right;

	/* 更改节点深度 */
	root->height = avl_node_max_height(root->left, root->right) + 1;
	left->height = avl_node_max_height(left->left, left->right) + 1;

	*target = left;
	return 0;
}

int avl_tree_left_rotate(TREE_NODE **target)
{
	TREE_NODE *root  = NULL;
	TREE_NODE *left  = NULL;
	TREE_NODE *right = NULL;

	/* 空节点不可旋转 */
	if (!target || !*target) {
		return -1;
	}

	root = *target;
	right = root->right;

	/* 左旋转的话，右节点不可能没有值 */
	if (!right) {
		return -1;
	}

	/*
	 * 1.目标节点变为右子树的左子节点
	 * 2.右子树的左节点变为目标节点的右子节点
	 * 3.更新节点深度信息
	 * 4.改变根节点为目标节点的右子节点
	 */

	left = right->left;
	right->left = root;
	root->right = left;

	/* 更改节点深度 */
	root->height = avl_node_max_height(root->left, root->right) + 1;
	right->height = avl_node_max_height(right->left, right->right) + 1;

	*target = right;
	return 0;
}

int avl_tree_lr_rotate(TREE_NODE **target)
{
	/*
	 * 先将左子节点左旋转，使其高深度的右子树转移至其左子树上，
	 * 这样就符合将目标节点进行右旋转的条件
	 */
	TREE_NODE *left = NULL;
	int ret = -1;

	if (!target || !*target) {
		return ret;
	}

	/* 目标的左子树不可为 NULL */
	left = (*target)->left;
	if (!left) {
		return -1;
	}

	/* 将左子节点进行左旋转 */
	ret = avl_tree_left_rotate(&left);
	if (ret < 0) {
		return ret;
	}

	/* 调整目标节点左节点 */
	(*target)->left = left;

	/* 将目标节点再进行一次右旋转 */
	return avl_tree_right_rotate(target);
}

int avl_tree_rl_rotate(TREE_NODE **target)
{
	/* 与左-右 双旋转镜像对应 */
	TREE_NODE *right = NULL;
	int ret = -1;

	if (!target || !*target) {
		return ret;
	}

	right = (*target)->right;
	if (!right) {
		return -1;
	}

	ret = avl_tree_right_rotate(&right);
	if (ret < 0) {
		return ret;
	}

	(*target)->right = right;
	return avl_tree_left_rotate(target);
}

int avl_tree_comp_height(TREE_NODE *left, TREE_NODE *right)
{
	if (!left && right) {
		return -1;
	} else if (left && !right) {
		return 1;
	} else if (!left && !right) {
		return 0;
	}

	/* 两个树节点都不为 NULL，比较树的高度 */
	if (left->height > right->height) {
		return 1;
	} else if (left->height < right->height) {
		return -1;
	}

	return 0;
}

int avl_tree_insert_node(TREE_NODE **node, char data)
{
	TREE_NODE *cur = NULL;
	int  ret  = -1;

	if (!node) {
		return -1;
	}

	cur = *node;

	if (!cur) {
		cur = malloc(sizeof(TREE_NODE));
		cur->data   = data;
		cur->left   = NULL;
		cur->right  = NULL;
		cur->height = 0;

		*node = cur;
		return 0;
	}

	/* 递归插入一个新节点 */
	if (data < cur->data) {
		ret = avl_tree_insert_node(&cur->left, data);
	} else if (data > cur->data) {
		ret = avl_tree_insert_node(&cur->right, data);
	} else {
		/* 值相等则直接覆盖 */
		cur->data = data;
	}

	if (ret < 0) {
		return ret;
	}

	ret = avl_tree_rebalance(node);
	return ret;
}

int avl_tree_delete_node(TREE_NODE **node, char data)
{
	TREE_NODE *cur = NULL;
	int ret = 0;

	if (!node) {
		return -1;
	}

	cur = *node;

	/* 节点不存在 */
	if (!cur) {
		return -1;
	}

	/* 递归搜索节点 */
	if (data < cur->data) {
		ret = avl_tree_delete_node(&cur->left, data);
	} else if (data > cur->data) {
		ret = avl_tree_delete_node(&cur->right, data);
	} else if (data == cur->data) {
		/*
		 * 找到对应的节点后
		 * 移除节点，同时要更新节点高度信息；
		 */
		if (cur->left && cur->right) {
			/*
			 * 被删除节点有两个子节点，将左子树的最大值（或者
			 * 右子树的最小值）和当前节点交换，从交换的子节点
			 * 开始向上回溯，如果发现不平衡的节点则重新旋转。
			 */
			ret = avl_tree_del_exchange(&cur->left, cur);
		} else if (cur->left && !cur->right) {
			/*
			 * 被删除的节点只有左节点，删除后对于当前节点而言，必定依旧维持
			 * 平衡状态
			 */
			TREE_NODE *l = cur->left;

			free(cur);
			*node = l;
			return 0;
		} else if (!cur->left && cur->right) {
			TREE_NODE *r = cur->right;

			free(cur);
			*node = r;
			return 0;
		} else {
			/* 删除独立的一个节点，直接移除并返回 */
			free(cur);
			*node = NULL;
			return 0;
		}
	}

	if (ret < 0) {
		return ret;
	}

	ret = avl_tree_rebalance(node);
	return ret;
}

void avl_tree_clear_nodes(TREE_NODE *node)
{
	if (!node) {
		return;
	}

	if (node->left) {
		avl_tree_clear_nodes(node->left);
	}
	if (node->right) {
		avl_tree_clear_nodes(node->right);
	}

	free(node);
}

int avl_tree_del_exchange(TREE_NODE **node, TREE_NODE *target)
{
	TREE_NODE *cur = NULL;
	TREE_NODE *l = NULL;
	TREE_NODE *r = NULL;

	int ret = 0;

	if (!node || !target) {
		return -1;
	}

	cur = *node;

	if (!cur) {
		return -1;
	}

	/* 一直向右节点搜索 */
	if (cur->right) {
		ret = avl_tree_del_exchange(&cur->right, target);
	} else {
		/*
		 * 1.直接交换数据；
		 * 2.如果当前节点有左子树，则将左子树变为当前节点;
		 */
		target->data = cur->data;

		if (cur->left) {
			*node = cur->left;
		} else {
			*node = NULL;
		}

		/* 释放当前节点 */
		free(cur);
	}

	if (ret < 0) {
		return ret;
	}

	ret = avl_tree_rebalance(node);
	return ret;
}

int avl_tree_rebalance(TREE_NODE **node)
{
	int ret = 0;
	int lh = -1;
	int rh = -1;

	TREE_NODE *cur = NULL;

	if (!node) {
		return -1;
	}

	cur = *node;

	/* 空节点不做处理 */
	if (!cur) {
		return 0;
	}

	/* 获取左右子树的高度 */
	lh = avl_node_height(cur->left);
	rh = avl_node_height(cur->right);

	/* 左子树和右子树高度相差大于 1，则进行旋转 */
	if ((lh - rh) > 1) {
		TREE_NODE *l = cur->left->left;
		TREE_NODE *r = cur->left->right;

		if (avl_tree_comp_height(l, r) >= 0) {
			/* 左边子树的左子树深度过深，执行右旋转 */
			ret = avl_tree_right_rotate(node);
		} else {
			/* 左边子树的右子树深度过深，执行左右双旋转 */
			ret = avl_tree_lr_rotate(node);
		}
	} else if ((rh - lh) > 1) {
		/* 与左子树插入镜像对应 */
		TREE_NODE *l = cur->right->left;
		TREE_NODE *r = cur->right->right;

		if (avl_tree_comp_height(l, r) <= 0) {
			ret = avl_tree_left_rotate(node);
		} else {
			ret = avl_tree_rl_rotate(node);
		}
	}

	if (ret < 0) {
		return ret;
	}

	/* height 为子树的高度 */
	cur->height = avl_node_max_height(cur->left, cur->right) + 1;
	return ret;
}

/*===========================================================================*/
