#include <stdio.h>
#include "rbtree.h"

/* 打印树信息 */
static void visit_tree(void *key, void *data, int bh);

struct key_value
{
    const char *key;
    const char *value;
} test_info[] = {
    { "A", "abc" },
    { "B", "bcd" },
    { "C", "cca" },
    { "D", "dec" },
    { "E", "ewe" },
    { "F", "father" },
    { "G", "go go go" },
    { "H", "hello world" },
    { "I", "I'm Herry" }
    // { "J", "Jerry" },
    // { "K", "Kao" }
};

int main(int argc, char *argv[])
{
    RB_TREE *tree = rb_create();
    int i = 0;
    int num = sizeof(test_info) / sizeof(struct key_value);

    for (; i < num; i++) {
        rb_insert(tree, (void *)(test_info[i].key), (void *)(test_info[i].value));
    }

#if 1
    /* 删除最简单的左节点或者右节点 */
    rb_delete(tree, "D", NULL);
#endif

    rb_iterate(tree, visit_tree);
    rb_destroy(tree);
    return 0;
}

void visit_tree(void *key, void *data, int bh)
{
    printf("key:%s, data = %s, bh = %d\n", (const char *)key, (const char *)data, bh);
}
