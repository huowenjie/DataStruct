#ifndef __GRAPH_H__
#define __GRAPH_H__

/**
 * 图 (graph) 的概念：
 *     一个图 G = (V, E) 由顶点的集合 V 和边的集合 E 构成；每一条边是一个点对 (v, w)，该点对
 * 包含于顶点集合 V 中。
 * 
 * 弧 (arc)：
 *     边又被称作弧；
 * 
 * 有向图 (digraph) 的概念：
 *     如果点对是有序的，那么所构成的图就被称为有向图 (digraph)
 * 
 * 邻接 (adjacent)：
 *     当且仅当边 (v, w) 属于集合 E 时，则说明顶点 v 和 w 邻接，也可说两个顶点互为邻接点；
 * 
 * 路径 (path)：
 *     存在一组顶点序列 w1，w2，w3，....wn，使得 (wi, wi+1) && 1 <= i < n 属于集合 E，
 * 那么称这个序列为一条路径；
 * 
 * 路径的长 (length)：
 *     一条路径的边数称为路径的长，等于 n - 1；如果路径不包含边，那么路径的长为 0；
 * 
 * 环 (loop)：
 *     一个从一个顶点 v 到它自身的路径，叫做一个环；
 * 
 * 圈 (cycle)：
 *     有向图中满足 w1 = wn 且长至少为 1 的路径；
 * 
 * 连通 (connected)：
 *     在一个无向图中，从每一个顶点到其他顶点都存在一条路径，则称该无向图是连通的；
 * 
 * 强连通 (strongly connected)：
 *     有向图如果是连通的，则称为强连通；
 * 
 * 基础图 (underlying graph)：
 *     有向图去掉弧上的方向属性，则称为有向图的基础图；
 * 
 * 弱连通 (weakly connected)：
 *     有向图不连通，但是其基础图连通，则称其为弱连通；
 * 
 * 完全图 (complete graph)：
 *     每一对顶点都存在一条边的图；
 * 
 * 邻接矩阵 (adjacency matrix)：
 *     一个二维数组 A，对于每条边 (u, v) 设 A[u][v] = 1，否则数组的元素为 0；优点是直观
 * 易懂，缺点是空间需求大；
 * 
 * 邻接表 (adjacency list)：
 *     对于每一个顶点，使用一个表存放所有邻接的顶点的结构。
 */

typedef struct graph_vertex_st GRAPH_VERTEX;
typedef struct graph_adjvex_st GRAPH_ADJTEX;

/* 邻接顶点 */
struct graph_adjvex_st
{
    /* 下一个节点 */
    GRAPH_ADJTEX *next;

    /* 邻接点索引，便于随机访问 */
    int index;
};

/* 图顶点 */
struct graph_vertex_st
{
    /* 邻接顶点列表的头尾节点 */
    GRAPH_ADJTEX *head;
    GRAPH_ADJTEX *tail;

    /* 邻接顶点数量 */
    int count;

    /* 顶点数据 */
    void *data;
};

/* 图 */
typedef struct graph_st
{
    GRAPH_VERTEX *vex_list;

    /* 顶点数量 */
    int number;

    /* 顶点最大数量 */
    int max_num;
} GRAPH;

/* 广度优先搜索树节点 */
typedef struct graph_bfs_node_st GRAPH_BFS_NODE;

/* 广度优先搜索树 */
typedef struct graph_bfs_tree_st GRAPH_BFS_TREE;

/* 深度优先搜索树节点 */
typedef struct graph_dfs_node_st GRAPH_DFS_NODE;

/* 深度优先搜索树森林 */
typedef struct graph_dfs_forest_st GRAPH_DFS_FOREST;

/* 创建图并指定最大容量 size，最少为 20，传 0 则按照默认大小 */
GRAPH *graph_create(int size);

/* 销毁图 */
void graph_destroy(GRAPH *graph);

/* 插入顶点，成功返回顶点索引，失败返回 -1 */
int graph_push_data(GRAPH *graph, void *data);

/* 指定当前顶点的邻接点索引，成功返回 0， 失败返回 -1 */
int graph_set_adjacent(GRAPH *graph, int cur, int dest);

/* 清理邻接表 */
void graph_clear_adjacent(GRAPH *graph);

/* 打印所有的邻接点 */
void graph_print(GRAPH *graph, const char *(*get_print_content)(void *));

/* 创建广度优先搜索树 */
GRAPH_BFS_TREE *graph_bfs_tree_create(const GRAPH *graph);

/* 销毁广度优先搜索树 */
void graph_bfs_tree_destroy(GRAPH_BFS_TREE *tree);

/* 从 src 开始执行广度优先搜索 */
int graph_bfs(const GRAPH *graph, GRAPH_BFS_TREE *tree, int src);

/* 广度优先搜索并打印源点到目标点的路径信息 */
void graph_bfs_print(GRAPH *graph, int src, int dest, const char *(*get_print_content)(void *));

/* 创建深度优先搜索森林 */
GRAPH_DFS_FOREST *graph_dfs_forest_create(const GRAPH *graph);

/* 销毁深度优先搜索森林 */
void graph_dfs_forest_destroy(GRAPH_DFS_FOREST *forest);

/**
 * 开始执行深度优先搜索
 * 
 * graph -- 图对象
 * forest -- 深度优先搜索树森林
 * index -- 当前节点的索引
 * visit_node_before -- 发现当前节点时的回调函数
 * visit_node_after -- 访问当前节点结束后的回调函数
 * args -- 在调用回调函数时传入的参数
 */
int graph_dfs(
    const GRAPH *graph,
    GRAPH_DFS_FOREST *forest,
    void (*visit_node_before)(void *, int),
    void (*visit_node_after)(void *, int),
    void *args
);

#endif /* __GRAPH_H__ */
