#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

typedef int val_type;
typedef struct Stack
{
    val_type *data_;
    size_t top_;
    size_t capacity_;
} Stack;

void stack_init(Stack *stack_p);

void stack_destory(Stack *stack_p);

void stack_push(Stack *stack_p, val_type x);

void stack_pop(Stack *stack_p);

val_type stack_top(Stack *stack_p);

bool stack_empty(Stack *stack_p);

int stacK_size(const Stack *stack_p);