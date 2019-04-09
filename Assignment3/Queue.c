#include "Queue.h"
#include <string.h>

//Very heavily inspired(copied) by: https://codereview.stackexchange.com/questions/141238/implementing-a-generic-queue-in-c
//TODO: Implement semaphores to make data structure thread safe
void Queue_Init(Queue *q, size_t memSize)
{
    q->count = 0;
    q->memSize = memSize;
    q->head = NULL;
    q->tail = NULL;
}

int enqueue(Queue *q, const void *data)
{
    node *newNode = (node *)malloc(sizeof(node));

    if(newNode == NULL) return -1;

    newNode->data = malloc(q->memSize);

    if(newNode->data == NULL)
    {
        free(newNode);
        return -1;
    }

    newNode->next = NULL;

    memcpy(newNode->data, data, q->memSize);

    if(q->count == 0) 
    {
        q->head = newNode;
        q->tail = newNode;
    }
    else
    {
        q->tail->next = newNode;
        q->tail = newNode;
    }
    q->count++;

    return 0;
}

void dequeue(Queue *q, void *data)
{
    if(q->count > 0)
    {
        node *tmp = q->head;
        memcpy(data, tmp->data, q->memSize);

        if(q->count > 1)
        {
            q->head = q->head->next;
        }
        else
        {
            q->head = NULL;
            q->tail = NULL;
        }

        q->count--;
        free(tmp->data);
        free(tmp);
    }
}

void Queue_Peek(const Queue *q, void *data)
{
    if(q->count > 0)
    {
        node *tmp = q->head;
        memcpy(data, tmp->data, q->memSize);
    }
}

void Queue_Clear(Queue *q)
{
    int val;
    while(q->count > 0)
    {
        dequeue(q, &val);
    }
}
int Queue_Count(const Queue *q) 
{
    int count = q->count;
    return count;
}