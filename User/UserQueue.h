#ifndef __USER_QUEUE_H_
#define __USER_QUEUE_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "Task_Manager.h"
#define QUEUE_MALLOC    pvPortMalloc
#define QUEUE_FREE      vPortFree

//typedef void* uQDataType;
#define uQDataType void*

// 节点结构体
struct QNode {
    uQDataType data;
    struct QNode *next;
};

// 队列结构体
struct Queue {
    struct QNode *head; // 队列头指针
    struct QNode *tail; // 队列尾指针
    int size;           // 队列节点大小
};


// 创建队列
struct Queue *Queue_create(void);
//销毁队列
void Queue_destroy(struct Queue *queue);

// 判断队列是否为空
int Queue_isEmpty(struct Queue *queue);
// 入队操作
void Queue_enqueue(struct Queue *queue, uQDataType data);
// 出队操作
int Queue_dequeue(struct Queue *queue, uQDataType *data);
// 获取队头元素
int Queue_front(struct Queue *queue, uQDataType *data);
// 获取队列长度
int Queue_size(struct Queue *queue);


#endif
