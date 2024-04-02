#include "List.h"

// 测试函数
int main()
{
    ListNode *head = NULL;

    // 头插法插入数据
    for (int i = 0; i < 5; ++i)
    {
        list_push_front(&head, i);
    }
    printf("链表头插法插入数据：\n");
    list_print(head);

    // 尾插法插入数据
    list_destory(&head); // 先销毁之前的链表
    for (int i = 0; i < 5; ++i)
    {
        list_push_back(&head, i);
    }
    printf("链表尾插法插入数据：\n");
    list_print(head);

    // 查找数据
    ListNode *search_result = list_find(head, 3);
    if (search_result != NULL)
        printf("查找到值为3的节点\n");
    else
        printf("未找到值为3的节点\n");

    // 删除节点
    list_erase(&head, search_result);
    printf("删除值为3的节点后的链表：\n");
    list_print(head);

    // 销毁链表
    list_destory(&head);
    printf("销毁链表后的情况：\n");
    list_print(head); // 应该输出NULL

    return 0;
}