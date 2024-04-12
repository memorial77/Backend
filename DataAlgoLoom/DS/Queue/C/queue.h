#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef int val_type;
typedef struct Node
{
    struct Node *next_;
    val_type data_;
} Node;

typedef struct Queue
{
    Node *head_;
    Node *tail_;
    size_t size_;
} Queue;

void queue_init(Queue *queue_p);

void queue_destory(Queue *queue_p);

void queue_push(Queue *pq, val_type val);

void queue_pop(Queue *queue_p);

val_type queue_front(Queue *queue_p);

val_type queue_back(Queue *queue_p);

bool queue_empty(Queue *queue_p);

size_t queue_size(Queue *queue_p);