#include "heap.h"

void heap_init(Heap *headp_p)
{
    assert(headp_p);
    headp_p->data_ = NULL;
    headp_p->size_ = 0;
    headp_p->capacity_ = 0;
}

void heap_destroy(Heap *headp_p)
{
    assert(headp_p);
    if (headp_p)
    {
        free(headp_p->data_);
        headp_p->data_ = NULL;
        headp_p->size_ = 0;
        headp_p->capacity_ = 0;
    }
}

bool heap_empty(Heap *heap_p)
{
    assert(heap_p);
    return heap_p->size_ == 0;
}

void heap_swap(val_type *val1_p, val_type *val2_p)
{
    assert(val1_p && val2_p);
    val_type temp = *val1_p;
    *val1_p = *val2_p;
    *val2_p = temp;
}

void adjust_up(val_type *data, size_t child)
{
    assert(data);
    // 获取父亲节点下标
    size_t parent = (child - 1) / 2;

    while (child > 0)
    {
        // 建大根堆
        if (data[child] > data[parent])
        {
            heap_swap(&data[child], &data[parent]);
            child = parent;
            parent = (child - 1) / 2;
        }
        else
            break;
    }
}

void adjust_down(val_type *data, size_t parent, size_t size)
{
    assert(data);
    size_t max_child = parent * 2 + 1;
    while (max_child < size)
    {
        // 如果右子节点存在且大于左子节点，则选择右子节点
        if (max_child + 1 < size && data[max_child + 1] > data[max_child])
            max_child = max_child + 1;

        if (data[max_child] > data[parent])
        {
            heap_swap(&data[max_child], &data[parent]);
            parent = max_child;
            max_child = parent * 2 + 1;
        }
        else
            break;
    }
}

void heap_push(Heap *heap_p, val_type val)
{
    assert(heap_p);
    // 空间容量为0或不够时开辟新空间
    if (heap_p->size_ == heap_p->capacity_)
    {
        size_t new_capacity = (heap_p->capacity_ == 0) ? 4 : heap_p->capacity_ * 2;
        val_type *temp = (val_type *)realloc(heap_p->data_, sizeof(val_type) * new_capacity);
        if (temp == NULL)
        {
            perror("realloc fail");
            exit(-1);
        }

        heap_p->data_ = temp;
        heap_p->capacity_ = new_capacity;
    }

    // 插入数据
    heap_p->data_[heap_p->size_++] = val;
    // 调整堆结构
    adjust_up(heap_p->data_, heap_p->size_ - 1);
}

void heap_pop(Heap *heap_p)
{
    assert(heap_p);
    assert(!heap_empty(heap_p));

    heap_swap(&heap_p->data_[0], &heap_p->data_[heap_p->size_ - 1]);
    heap_p->size_--;

    adjust_down(heap_p->data_, 0, heap_p->size_);
}

val_type heap_top(Heap *heap_p)
{
    assert(heap_p);
    assert(!heap_empty(heap_p));

    return heap_p->data_[0];
}

size_t heap_size(Heap *heap_p)
{
    assert(heap_p);
    return heap_p->size_;
}

void print_heap(Heap *heap_p)
{
    printf("Heap: [");
    for (size_t i = 0; i < heap_size(heap_p); i++)
    {
        printf("%d", heap_p->data_[i]);
        if (i < heap_size(heap_p) - 1)
            printf(", ");
    }
    printf("]\n");
}

// 堆排序
void headp_sort(val_type *data, size_t size)
{
    // 建堆（大根堆）O(n)
    for (int i = (size - 1 - 1) / 2; i >= 0; i--)
        adjust_down(data, i, size);

    // 排序 nlog(n)
    int count = size;
    for (int i = 0; i < size; i++)
    {
        heap_swap(&data[0], &data[count - 1]);
        adjust_down(data, 0, count - 1);
        count--;
    }
}

val_type *top_k_min(val_type *arr, size_t size, size_t k)
{
    val_type *data = (val_type *)malloc(sizeof(val_type) * k);
    for (int i = 0; i < k; i++)
        data[i] = arr[i];

    // 建大根堆（不断去除大值留下小值）
    for (int i = (k - 1 - 1) / 2; i >= 0; i--)
        adjust_down(data, i, k);

    for (int i = k; i < size; i++)
    {
        // 若读取的值小于堆顶则进行替换去除大值并调整结构
        if (arr[i] < data[0])
        {
            data[0] = arr[i];
            adjust_down(data, 0, k);
        }
    }
    
    return data;
}