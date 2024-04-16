#include "heap.h"
#include <time.h>

void heap_test()
{
    // 初始化堆
    Heap heap;
    heap_init(&heap);

    // 向堆中插入元素
    printf("Pushing elements into the heap...\n");
    heap_push(&heap, 10);
    heap_push(&heap, 20);
    heap_push(&heap, 15);
    heap_push(&heap, 30);
    heap_push(&heap, 25);
    print_heap(&heap);

    // 获取堆的大小和顶部元素
    printf("Heap size: %zu\n", heap_size(&heap));
    printf("Top element: %d\n", heap_top(&heap));

    // 从堆中删除顶部元素
    printf("Popping top element from the heap...\n");
    heap_pop(&heap);
    print_heap(&heap);

    // 获取更新后的堆的大小和顶部元素
    printf("Heap size: %zu\n", heap_size(&heap));
    printf("Top element: %d\n", heap_top(&heap));

    // 销毁堆
    heap_destroy(&heap);
}

void sort_test()
{
    // 测试用例
    val_type data[] = {4, 10, 3, 5, 1, 7, 9};
    int size = sizeof(data) / sizeof(data[0]);

    printf("Original array: ");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", data[i]);
    }
    printf("\n");

    // 调用堆排序函数
    headp_sort(data, size);

    printf("Sorted array: ");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", data[i]);
    }
    printf("\n");
}

void top_test()
{
    // 设置随机数种子
    srand(time(NULL));

    // 生成随机数组
    size_t size = 20; // 数组大小
    val_type *arr = (val_type *)malloc(sizeof(val_type) * size);
    printf("Original Array: ");
    for (size_t i = 0; i < size; ++i) {
        arr[i] = rand() % 100; // 生成 0到99之间的随机数
        printf("%d ", arr[i]);
    }
    printf("\n");

    size_t k = 5; // 选择前 k 小的元素

    // 调用函数
    val_type *result = top_k_min(arr, size, k);

    // 打印结果
    printf("Top %zu min elements: ", k);
    for (size_t i = 0; i < k; ++i) {
        printf("%d ", result[i]);
    }
    printf("\n");

    // 释放内存
    free(arr);
    free(result);
}

int main()
{
    top_test();
    return 0;
}