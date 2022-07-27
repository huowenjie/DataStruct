#include <stdio.h>
#include <string.h>
#include "graph.h"

const char *string_list[] = {
    "内裤", /* 0 */
    "袜子", /* 1 */
    "裤子", /* 2 */
    "腰带", /* 3 */
    "鞋子", /* 4 */
    "手表", /* 5 */
    "衬衣", /* 6 */
    "领带", /* 7 */
    "夹克", /* 8 */
    NULL
};

void build_topology_data(GRAPH *graph)
{
    const char *str = NULL;
    int i = 0;
    
    while ((str = string_list[i])) {
        graph_push_data(graph, (void *)str);
        i++;
    }

    /* 设置每个节点的邻接点，使之构成一个有向无环图 */

    graph_set_adjacent(graph, 0, 2);
    graph_set_adjacent(graph, 0, 4);

    graph_set_adjacent(graph, 1, 4);

    graph_set_adjacent(graph, 2, 3);
    graph_set_adjacent(graph, 2, 4);

    graph_set_adjacent(graph, 3, 8);

    graph_set_adjacent(graph, 6, 3);
    graph_set_adjacent(graph, 6, 7);

    graph_set_adjacent(graph, 7, 8);
}

void print_found(void *args, int index) {
    printf("%s\n", string_list[index]);
}

void test_dfs()
{
    GRAPH *graph = graph_create(0);
    GRAPH_DFS_FOREST *forest = NULL;

    /* 构建拓扑数据 */
    build_topology_data(graph);

    /* 创建深度优先搜索森林 */
    forest = graph_dfs_forest_create(graph);

    /* 深度优先搜索 */
    graph_dfs(graph, forest, NULL, print_found, NULL);

    graph_dfs_forest_destroy(forest);
    graph_clear_adjacent(graph);
    graph_destroy(graph);
}