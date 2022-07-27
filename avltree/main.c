#include <stdio.h>
#include "avltree.h"

#ifdef WIN32
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif /* _DEBUG */
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif /* _DEBUG */

#ifdef _DEBUG
#define CHECK_START _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
#define CHECK_START
#endif
#endif /* WIN32 */

void show_tree_before(TREE_NODE *node)
{
	if (!node)
	{
		return;
	}

	show_tree_before(node->left);
	printf("%c(%d) ", node->data, node->height);
	show_tree_before(node->right);
}

int main(int argc, char *argv[])
{
	AVL_TREE *tree = NULL;

#ifdef WIN32
	CHECK_START;
#endif

	tree = avl_tree_create();

#if 0
	/* 左旋转测试 */
	avl_tree_insert(tree, 'I');
	avl_tree_insert(tree, 'H');
	avl_tree_insert(tree, 'G');
	avl_tree_insert(tree, 'F');
	avl_tree_insert(tree, 'E');
	avl_tree_insert(tree, 'D');
	avl_tree_insert(tree, 'C');
	avl_tree_insert(tree, 'B');
	avl_tree_insert(tree, 'A');
#elif 0
	/* 右旋转测试 */
	avl_tree_insert(tree, 'A');
	avl_tree_insert(tree, 'B');
	avl_tree_insert(tree, 'C');
	avl_tree_insert(tree, 'D');
	avl_tree_insert(tree, 'E');
	avl_tree_insert(tree, 'F');
	avl_tree_insert(tree, 'G');
	avl_tree_insert(tree, 'H');
	avl_tree_insert(tree, 'I');
#elif 0
	/* 左-右双旋转测试 */
	avl_tree_insert(tree, 'F');
	avl_tree_insert(tree, 'B');
	avl_tree_insert(tree, 'G');
	avl_tree_insert(tree, 'A');
	avl_tree_insert(tree, 'D');
	avl_tree_insert(tree, 'C');
	avl_tree_insert(tree, 'E');
	avl_tree_insert(tree, 'H');
	avl_tree_insert(tree, 'I');
#elif 0
	/* 右-左双旋转测试 */
	avl_tree_insert(tree, 'H');
	avl_tree_insert(tree, 'D');
	avl_tree_insert(tree, 'K');
	avl_tree_insert(tree, 'I');
	avl_tree_insert(tree, 'W');
	avl_tree_insert(tree, 'J');
	avl_tree_insert(tree, 'F');

#elif 0
	/* 删除操作 */
	avl_tree_insert(tree, 'A');
	avl_tree_insert(tree, 'B');
	avl_tree_insert(tree, 'C');
	avl_tree_insert(tree, 'D');
	avl_tree_insert(tree, 'E');
	avl_tree_insert(tree, 'F');
	avl_tree_insert(tree, 'G');
	avl_tree_insert(tree, 'H');
	avl_tree_insert(tree, 'I');
	avl_tree_insert(tree, 'J');
	avl_tree_insert(tree, 'K');
	avl_tree_insert(tree, 'L');
	avl_tree_insert(tree, 'M');
	avl_tree_insert(tree, 'N');
	avl_tree_insert(tree, 'O');

	show_tree_before(tree->root);
	printf("\n");

	/* 删除节点 */
	avl_tree_delete(tree, 'D');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'G');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'H');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'E');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'L');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'M');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'N');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'A');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'C');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'J');

	show_tree_before(tree->root);
	printf("\n");

	avl_tree_delete(tree, 'I');
	avl_tree_delete(tree, 'K');
	avl_tree_delete(tree, 'F');
	avl_tree_delete(tree, 'B');
	avl_tree_delete(tree, 'O');

#elif 1
	/* 相同节点插入删除 */
	avl_tree_insert(tree, 'A');
	avl_tree_insert(tree, 'A');
	avl_tree_insert(tree, 'B');
	avl_tree_insert(tree, 'B');
	avl_tree_insert(tree, 'B');
	avl_tree_insert(tree, 'C');
	avl_tree_insert(tree, 'D');

	show_tree_before(tree->root);
	printf("\n");

	/* 删除节点 */
	avl_tree_delete(tree, 'A');
#endif
	show_tree_before(tree->root);
	avl_tree_clear(tree);
	avl_tree_destroy(tree);

	getchar();
	return 0;
}
