#include "stack.h"

void stack_init(Stack *stack_p)
{
    assert(stack_p);
    stack_p->data_ = NULL;
    stack_p->top_ = 0;
    stack_p->capacity_ = 0;
}

void stack_destory(Stack *stack_p)
{
    assert(stack_p);
    if (stack_p)
        free(stack_p->data_);
    stack_p->data_ = NULL;
    stack_p->top_ = 0;
    stack_p->capacity_ = 0;
}

void stack_push(Stack *stack_p, val_type val)
{
    assert(stack_p);
    if (stack_p->top_ == stack_p->capacity_)
    {
        size_t new_capacity = (stack_p->capacity_ == 0) ? 4 : stack_p->capacity_ * 2;
        val_type *temp_data = (val_type *)realloc(stack_p->data_, sizeof(val_type) * new_capacity);
        if (temp_data == NULL)
        {
            perror("realloc fail");
            exit(-1);
        }

        stack_p->data_ = temp_data;
        stack_p->capacity_ = new_capacity;
    }

    stack_p->data_[stack_p->top_++] = val;
}

void stack_pop(Stack *stack_p)
{
    assert(stack_p);
    assert(!stack_empty(stack_p));
    stack_p->top_--;
}

val_type stack_top(Stack *stack_p)
{
    assert(stack_p);
    assert(!stack_empty(stack_p));
    return stack_p->data_[stack_p->top_ - 1];
}

bool stack_empty(Stack *stack_p)
{
    assert(stack_p);
    return stack_p->top_ == 0;
}

int stacK_size(const Stack *stack_p)
{
    assert(stack_p);
    return stack_p->top_;
}