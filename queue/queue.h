#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stddef.h>

/* 队列模板，第一个参数是命名空间，第二个参数是类型前缀，第三个参数是元素类型 */
#define QUEUE_DEFINE(ns, nscp, type) \
    typedef struct ns##_queue_st{ \
        type *elems; \
        int size; \
        int num; \
        int hpos; \
    } nscp##_QUEUE; \
    \
    extern int ns##_queue_enqueue(nscp##_QUEUE *queue, type val); \
    extern int ns##_queue_dequeue(nscp##_QUEUE *queue); \
    extern type ns##_queue_head(nscp##_QUEUE *queue);

/* 队列实现 */
#define QUEUE_IMPLEMENT(ns, nscp, type, terr) \
    int ns##_queue_enqueue(nscp##_QUEUE *queue, type val) { \
        type *val_list = NULL; \
        int tail = 0; \
        int num  = 0; \
        int hpos = 0; \
        int size = 0; \
        \
        if (!queue) { \
            return -1; \
        } \
        \
        val_list = queue->elems; \
        \
        if (!val_list) { \
            return -1; \
        } \
        \
        num  = queue->num; \
        hpos = queue->hpos; \
        size = queue->size; \
        \
        if (queue->num >= size) { \
            return -1; \
        } \
        \
        tail = (!num) ? hpos : ((hpos + num) % size); \
        \
        val_list[tail] = val; \
        queue->num++; \
        return 0; \
    } \
    \
    int ns##_queue_dequeue(nscp##_QUEUE *queue) { \
	    type *val_list = NULL; \
        int num  = 0; \
        int hpos = 0; \
        \
        if (!queue) { \
            return -1; \
        } \
        \
        val_list = queue->elems; \
        \
        if (!val_list) { \
            return -1; \
        } \
        \
        num  = queue->num; \
        hpos = queue->hpos; \
        \
        if (!num) { \
            return -1; \
        } \
        \
        hpos = (hpos + 1) % queue->size; \
        queue->hpos = hpos; \
        queue->num = num - 1; \
        \
        return 0; \
    } \
    \
    type ns##_queue_head(nscp##_QUEUE *queue) { \
        int hpos = 0; \
        \
        if (!queue) { \
            return terr; \
        } \
        \
        if (!queue->num) { \
            return terr; \
        } \
        \
        hpos = queue->hpos; \
        return queue->elems[hpos]; \
    }

#endif /* __QUEUE_H__ */
