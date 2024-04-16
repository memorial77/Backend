#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef int val_type;
typedef struct Heap
{
    val_type *data_;
    size_t size_;
    size_t capacity_;
} Heap;

// 初始化堆结构
void heap_init(Heap *heap_p);

// 销毁堆结构
void heap_destroy(Heap *headp_p);

// 判空
bool heap_empty(Heap* heap_p);

// 交换数据
void heap_swap(val_type *val1_p, val_type *val2_p);

// 向上调整
void adjust_up(val_type *data, size_t child);

// 向下调整
void adjust_down(val_type *data, size_t parent, size_t size);

// 插入数据
void heap_push(Heap *heap_p, val_type val);

// 删除数据
void heap_pop(Heap *heap_p);

// 获取堆顶数据
val_type heap_top(Heap* heap_p);

// 获取堆数据个数
size_t heap_size(Heap* heap_p);

// 打印堆中的元素
void print_heap(Heap *heap_p);

// 堆排序
void headp_sort(val_type* data, size_t size);

// topk(min)
val_type *top_k_min(val_type *arr, size_t size, size_t k);