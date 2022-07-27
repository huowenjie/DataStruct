#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "../queue/queue.h"

#define GRAPH_DEFAULT_SIZE 512

/* 搜索树的节点颜色 */
#define GRAPH_BFS_COLOR_WHITE 0
#define GRAPH_BFS_COLOR_GRAY 1
#define GRAPH_BFS_COLOR_BLACK 2

struct graph_bfs_node_st
{
    int index;
    int parent;
    int color;
    int distances;
};

struct graph_bfs_tree_st
{
    GRAPH_BFS_NODE *nodes;
    int count;
};

struct graph_dfs_node_st
{
    int index;
    int parent;
    int color;
    int ts_find;
    int ts_ok;
};

struct graph_dfs_forest_st
{
    GRAPH_DFS_NODE *nodes;

    int count;

    /* 全局时间戳 */
    int time;
};

/* 定义顶点队列 */
QUEUE_DEFINE(graph, GRAPH, int);

/* 为图顶点插入邻接顶点 */

GRAPH *graph_create(int size)
{
    GRAPH *graph = NULL;
    GRAPH_VERTEX *list = NULL;

    if (size <= 0) {
        size = GRAPH_DEFAULT_SIZE;
    }

    graph = malloc(sizeof(GRAPH));
    memset(graph, 0, sizeof(GRAPH));

    list = malloc(size * sizeof(GRAPH_VERTEX));
    memset(graph, 0, sizeof(GRAPH));

    graph->number = 0;
    graph->max_num = size;
    graph->vex_list = list;

    return graph;
}

void graph_destroy(GRAPH *graph)
{
    if (!graph) {
        return;
    }

    graph->max_num = 0;
    graph->number = 0;

    if (graph->vex_list) {
        free(graph->vex_list);
    }

    free(graph);
}

int graph_push_data(GRAPH *graph, void *data)
{
    GRAPH_VERTEX *list = NULL;
    int count = 0;

    if (!graph) {
        return -1;
    }

    list = graph->vex_list;
    count = graph->number;

    if (count >= graph->max_num) {
        return -1;
    }

    list[count].data = data;
    graph->number++;
    return count;
}

int graph_set_adjacent(GRAPH *graph, int cur, int dest)
{
    GRAPH_VERTEX *list = NULL;
    GRAPH_VERTEX *vertex = NULL;
    GRAPH_ADJTEX *node = NULL;

    if (!graph) {
        return -1;
    }

    if (cur >= graph->number || dest >= graph->number) {
        return -1;
    }

    list = graph->vex_list;
    vertex = list + cur;
    node = vertex->head;

    for (; node != NULL && node != vertex->tail; node = node->next) {
        if (node->index == dest) {
            return -1;
        }
    }

    if (node && node->index == dest) {
        return -1;
    }

    /* 创建新的邻接顶点并设置信息 */
    node = malloc(sizeof(GRAPH_ADJTEX));
    memset(node, 0, sizeof(GRAPH_ADJTEX));

    node->next = NULL;
    node->index = dest;

    /* 将新创建的节点设置为尾结点 */
    if (!vertex->head) {
        vertex->head = node;
        vertex->tail = node;
    } else {
        vertex->tail->next = node;
        vertex->tail = node;
    }

    vertex->count++;
    return 0;
}

void graph_clear_adjacent(GRAPH *graph)
{
    GRAPH_VERTEX *list = NULL;
    GRAPH_ADJTEX *node = NULL;

    int index = 0;
    int count = 0;

    if (!graph) {
        return;
    }

    list = graph->vex_list;

    /* 依次释放所有的邻接点 */
    for (count = graph->number; index < count; index++) {
        GRAPH_ADJTEX *tmp = NULL;
        node = list[index].head;

        while (node) {
            tmp = node->next;
            free(node);
            node = tmp;
        }

        list[index].count = 0;
    }
}

/**
 * 格式：
 * 
 * 第 1 个节点，地址 0xFFFFAAAABBBBCCCC
 *     邻接点 % 个：
 *         (1) index = 2, 0xFFFFAAAABBBBDDDD
 * 数据：
 * xxxxxxxx
 * 
 * 第 2 个节点，地址 xxxxx
 *  ...
 */
void graph_print(GRAPH *graph, const char *(*get_print_content)(void *))
{
    GRAPH_VERTEX *list = NULL;
    GRAPH_ADJTEX *node = NULL;

    int index = 0;
    int count = 0;

    if (!graph) {
        return;
    }

    printf("------------------------------------------------------------\n");

    list = graph->vex_list;

    /* 依次释放所有的邻接点 */
    for (count = graph->number; index < count; index++) {
        int i = 1;

        node = list[index].head;

        printf("第 %d 个节点，地址 %p \n", index + 1, list + index);
        
        if (get_print_content) {
            printf("数据：\n");
            printf("%s\n", get_print_content(list[index].data));
        }

        printf("    邻接点 %d 个：\n", list[index].count);

        if (!get_print_content) {
            while (node) {
                printf("        (%d) index = %d, %p\n", i++, node->index, node);
                node = node->next;
            }
        } else {    
            while (node) {
                printf("        (%d) index = %d, %s, %p\n", i++, node->index, get_print_content(list[node->index].data), node);
                node = node->next;
            }
        }

        printf("------------------------------------------------------------\n");
    }
}

/*---------------------------------------------------------------------------*/

GRAPH_BFS_TREE *graph_bfs_tree_create(const GRAPH *graph)
{
    GRAPH_BFS_TREE *tree = NULL;
    GRAPH_BFS_NODE *nodes = NULL;
    int i = 0;
    int j = 0;

    if (!graph) {
        return NULL;
    }

    tree = malloc(sizeof(GRAPH_BFS_TREE));
    memset(tree, 0, sizeof(GRAPH_BFS_TREE));

    j = graph->number;
    nodes = malloc(j * sizeof(GRAPH_BFS_NODE));
    memset(nodes, 0, j * sizeof(GRAPH_BFS_NODE));

    for (; i < j; i++) {
        nodes[i].index = i;
        nodes[i].parent = -1;
        nodes[i].distances = -1;
        nodes[i].color = GRAPH_BFS_COLOR_WHITE;
    }

    tree->count = j;
    tree->nodes = nodes;
    return tree;
}

void graph_bfs_tree_destroy(GRAPH_BFS_TREE *tree)
{
    if (tree) {
        if (tree->nodes) {
            free(tree->nodes);
        }
        free(tree);
    }
}

int graph_bfs(const GRAPH *graph, GRAPH_BFS_TREE *tree, int src)
{
    GRAPH_BFS_NODE *nodes = NULL;
    GRAPH_VERTEX *vlist = NULL;

    int *buf = NULL;
    int count = 0;

    GRAPH_QUEUE queue = {
        NULL, 0, 0, 0
    };

    if (!graph || !tree) {
        return -1;
    }

    vlist = graph->vex_list;
    nodes = tree->nodes;
    nodes[src].color = GRAPH_BFS_COLOR_GRAY;
    nodes[src].distances = 0;
    nodes[src].parent = -1;

    count = tree->count;

    /* 创建队列缓冲区 */
    buf = malloc(sizeof(int) * count);
    memset(buf, 0, sizeof(int) * count);

    /* 构建队列 */
    queue.elems = buf;
    queue.hpos = 0;
    queue.num = 0;
    queue.size = tree->count;

    /* 将每次发现的节点入队，此时先将源节点入队 */
    graph_queue_enqueue(&queue, src);

    /* 依次搜索图节点 */
    while (queue.num > 0) {
        GRAPH_ADJTEX *adj = NULL;

        /* 获取队列结点 */
        int u = graph_queue_head(&queue);

        /* 出队 */
        graph_queue_dequeue(&queue);

        /* 依次访问结点的邻接点 */
        adj = vlist[u].head;
        while (adj) {
            int index = adj->index;
            GRAPH_BFS_NODE *node = nodes + index;

            if (node->color == GRAPH_BFS_COLOR_WHITE) {
                node->color = GRAPH_BFS_COLOR_GRAY;
                node->distances = nodes[u].distances + 1;
                node->parent = u;

                /* 新发现一个节点将其放入队列 */
                graph_queue_enqueue(&queue, index);
            }
            adj = adj->next;
        }

        nodes[u].color = GRAPH_BFS_COLOR_BLACK;
    }

    free(buf);
    return 0;
}

/* 打印路径 */
static void graph_print_path(
    GRAPH *graph,
    GRAPH_BFS_TREE *tree,
    int src,
    int dest,
    const char *(*get_print_content)(void *)
)
{
    GRAPH_VERTEX *vlist = NULL;
    GRAPH_BFS_NODE *nodes = NULL;

    if (!graph || !tree || !get_print_content) {
        return;
    }

    vlist = graph->vex_list;
    nodes = tree->nodes;

    if (src == dest) {
        printf("%s -- distance = %d\n", get_print_content(vlist[src].data), nodes[dest].distances);
    } else if (nodes[dest].parent == -1) {
        printf("路径不存在\n");
    } else {
        graph_print_path(graph, tree, src, nodes[dest].parent, get_print_content);
        printf("%s -- distance = %d\n", get_print_content(vlist[dest].data), nodes[dest].distances);
    }
}

void graph_bfs_print(GRAPH *graph, int src, int dest, const char *(*get_print_content)(void *))
{
    GRAPH_BFS_TREE *tree = NULL;

    if (!graph) {
        return;
    }

    /* 首先创建一颗广度优先搜索树 */
    tree = graph_bfs_tree_create(graph);
    if (!tree) {
        return;
    }

    /* 执行广度优先搜索 */
    graph_bfs(graph, tree, src);

    /* 根据生成的广度优先树打印最短路径 */
    graph_print_path(graph, tree, src, dest, get_print_content);

    if (tree) {
        graph_bfs_tree_destroy(tree);
    }
}

GRAPH_DFS_FOREST *graph_dfs_forest_create(const GRAPH *graph)
{
    GRAPH_DFS_FOREST *forest = NULL;
    GRAPH_DFS_NODE *nodes = NULL;
    int i = 0;
    int j = 0;

    if (!graph) {
        return NULL;
    }

    forest = malloc(sizeof(GRAPH_DFS_FOREST));
    memset(forest, 0, sizeof(GRAPH_DFS_FOREST));

    j = graph->number;
    nodes = malloc(j * sizeof(GRAPH_DFS_NODE));
    memset(nodes, 0, j * sizeof(GRAPH_DFS_NODE));

    for (; i < j; i++) {
        GRAPH_DFS_NODE *node = nodes + i;
        node->index = i;
        node->parent = -1;
        node->color = GRAPH_BFS_COLOR_WHITE;
        node->ts_find = 0;
        node->ts_ok = 0;
    }

    forest->nodes = nodes;
    forest->count = j;
    forest->time = 0;
    return forest;
}

void graph_dfs_forest_destroy(GRAPH_DFS_FOREST *forest)
{
    if (forest) {
        if (forest->nodes) {
            free(forest->nodes);
        }
        free(forest);
    }
}

/**
 * graph -- 图对象
 * forest -- 深度优先搜索树森林
 * index -- 当前节点的索引
 * visit_node_before -- 发现当前节点时的回调函数
 * visit_node_after -- 访问当前节点结束后的回调函数
 * args -- 在调用回调函数时传入的参数
 */
static int graph_dfs_visit(
    const GRAPH *graph,
    GRAPH_DFS_FOREST *forest,
    int index,
    void (*visit_node_before)(void *, int),
    void (*visit_node_after)(void *, int),
    void *args)
{
    GRAPH_DFS_NODE *nodes = NULL;
    GRAPH_DFS_NODE *node = NULL;

    GRAPH_VERTEX *vertex = NULL;
    GRAPH_ADJTEX *next = NULL;

    if (!graph || !forest) {
        return -1;
    }

    forest->time += 1;
    nodes = forest->nodes;
    node = nodes + index;

    /* 标记发现的节点 */
    node->color = GRAPH_BFS_COLOR_GRAY;
    node->ts_find = forest->time;

    if (visit_node_before) {
        visit_node_before(args, index);
    }

    /* 获取图顶点 */
    vertex = graph->vex_list + index;

    /* 获取邻接点 */
    next = vertex->head;

    /* 搜索当前节点的邻接点 */
    while (next) {
        int cur = next->index;
        GRAPH_DFS_NODE *tmp = nodes + cur;

        if (tmp->color == GRAPH_BFS_COLOR_WHITE) {
            tmp->parent = index;

            /* 继续查找邻接点的邻接点，依次递归进去 */
            if (graph_dfs_visit(graph, forest, cur, visit_node_before, visit_node_after, args)) {
                return -1;
            }
        }

        next = next->next;
    }

    forest->time += 1;
    node->ts_ok = forest->time;
    node->color = GRAPH_BFS_COLOR_BLACK;

    if (visit_node_after) {
        visit_node_after(args, index);
    }

    return 0;
}

int graph_dfs(
    const GRAPH *graph,
    GRAPH_DFS_FOREST *forest,
    void (*visit_node_before)(void *, int),
    void (*visit_node_after)(void *, int),
    void *args
)
{
    int i = 0;
    int count = 0;
    GRAPH_DFS_NODE *nodes = NULL;

    if (!graph || !forest) {
        return -1;
    }

    forest->time = 0;
    nodes = forest->nodes;

    for (count = forest->count; i < count; i++) {
        GRAPH_DFS_NODE *node = nodes + i;
        if (node->color == GRAPH_BFS_COLOR_WHITE) {
            if (graph_dfs_visit(graph, forest, i, visit_node_before, visit_node_after, args) != 0) {
                return -1;
            }
        }
    }

    return 0;
}

/* 实现顶点队列 */
QUEUE_IMPLEMENT(graph, GRAPH, int, 0);
