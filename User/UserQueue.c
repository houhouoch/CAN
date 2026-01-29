#include "UserQueue.h"




// 创建节点
static struct QNode *Queue_createNode(uQDataType data) {
    struct QNode *newNode = (struct QNode *)QUEUE_MALLOC(sizeof(struct QNode));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// 创建队列
struct Queue *Queue_create(void) {
    struct Queue *queue = (struct Queue *)QUEUE_MALLOC(sizeof(struct Queue));
    queue->head = queue->tail = NULL;
    return queue;
}

//销毁队列
void Queue_destroy(struct Queue *queue) {
    while(Queue_isEmpty(queue) == 0) {
        uQDataType data = {0};
        Queue_dequeue(queue, &data);
    }
    QUEUE_FREE(queue);
    queue = NULL;
}

// 判断队列是否为空
int Queue_isEmpty(struct Queue *queue) {
    return queue->head == NULL;
}

// 入队操作
void Queue_enqueue(struct Queue *queue, uQDataType data) {
    struct QNode *newNode = Queue_createNode(data);
    if(Queue_isEmpty(queue)) {
        queue->head = queue->tail = newNode;
        return;
    }
    queue->tail->next = newNode;
    queue->tail = newNode;
}

// 出队操作
int Queue_dequeue(struct Queue *queue, uQDataType *data) {
    if(Queue_isEmpty(queue)) {
        return -1;
    }
    *data = queue->head->data;
    struct QNode *temp = queue->head;
    queue->head = queue->head->next;
    vPortFree(temp);
    return 0;
}

// 获取队头元素
int Queue_front(struct Queue *queue, uQDataType *data) {
    if(Queue_isEmpty(queue)) {
        return -1;
    }
    *data = queue->head->data;
    return 0;
}

// 获取队列长度
int Queue_size(struct Queue *queue) {
    int count = 0;
    struct QNode *current = queue->head;
    while(current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}








