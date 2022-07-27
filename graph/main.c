#include <stdio.h>

/* 广度优先搜索 */
extern void test_bfs();

/* 深度优先搜索测试 */
extern void test_dfs();

int main(int argc, char *argv[]) 
{
#if 0
    test_bfs();
#else
    test_dfs();
#endif
    return 0;
}
