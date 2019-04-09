#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include <stdlib.h>

typedef struct Node
{
    void *data;
    struct Node *next;
} node;

typedef struct QueueL
{
    size_t count;
    size_t memSize;
    node *head;
    node *tail;
} Queue;

void Queue_Init(Queue *q, size_t memSize);
int enqueue(Queue *, const void *);
void dequeue(Queue *, void *);
void Queue_Peek(const Queue *, void *);
void Queue_Clear(Queue *);
int Queue_Count(const Queue *);

#endif //QUEUE_H_INCLUDED