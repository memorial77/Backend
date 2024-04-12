#include "queue.h"

void queue_init(Queue *queue_p)
{
    assert(queue_p);
    queue_p->head_ = queue_p->tail_ = NULL;
    queue_p->size_ = 0;
}

void queue_destory(Queue *queue_p)
{
    assert(queue_p);
    Node *cur = queue_p->head_;

    while (cur != NULL)
    {
        Node *del = cur;
        cur = cur->next_;
        free(del);
    }

    queue_p->head_ = queue_p->tail_ = NULL;
    queue_p->size_ = 0;
}

void queue_push(Queue *queue_p, val_type val)
{
    assert(queue_p);

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        perror("malloc fail");
        exit(-1);
    }
    new_node->data_ = val;
    new_node->next_ = NULL;

    if (queue_p == NULL)
        queue_p->head_ = queue_p->tail_ = new_node;
    else
    {
        queue_p->tail_->next_ = new_node;
        queue_p->tail_ = new_node;
    }

    queue_p->size_++;
}

void queue_pop(Queue *queue_p)
{
    assert(queue_p);

    if (queue_p->head_->next_ == NULL)
    {
        free(queue_p->head_);
        queue_p->head_ = queue_p->tail_ = NULL;
    }
    else
    {
        Node *del = queue_p->head_;
        queue_p->head_ = del->next_;
        free(del);
    }

    queue_p->size_--;
}

val_type queue_front(Queue *queue_p)
{
    assert(queue_p);
    assert(!queue_empty(queue_p));

    return queue_p->head_->data_;
}
val_type queue_back(Queue *queue_p)
{
    assert(queue_p);
    assert(!queue_empty(queue_p));

    return queue_p->tail_->data_;
}

bool queue_empty(Queue *queue_p)
{
    assert(queue_p);
    return queue_p->size_ == 0;
}

size_t queue_size(Queue *queue_p)
{
    assert(queue_p);
    return queue_p->size_;
}