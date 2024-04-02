#include "List.h"

// 打印链表
void list_print(ListNode *p_head)
{
    ListNode *cur = p_head;
    while (cur != NULL)
    {
        printf("%d -> ", cur->data_);
        cur = cur->next_; // 更新cur指针向后移动
    }
    printf("NULL\n");
}

// 获取链表新节点
ListNode *get_list_node(list_data_type val)
{
    ListNode *new_node = (ListNode *)malloc(sizeof(ListNode));
    if (new_node == NULL)
    {
        perror("Get new node fail.\n");
        exit(-1);
    }
    new_node->data_ = val;
    new_node->next_ = NULL;
    return new_node;
}

// 链表头插节点
void list_push_front(ListNode **pp_head, list_data_type val)
{
    assert(pp_head);
    // 获取新节点
    ListNode *new_node = get_list_node(val);
    // 情况分为空链表和非空链表
    if (*pp_head == NULL)
        *pp_head = new_node;
    else
    {
        ListNode *first_node = *pp_head;
        *pp_head = new_node;
        new_node->next_ = first_node;
    }
}

// 尾插数据
void list_push_back(ListNode **pp_head, list_data_type val)
{
    assert(pp_head);
    // 获取新节点
    ListNode *new_node = get_list_node(val);
    // 情况分为空链表和非空链表
    if (*pp_head == NULL)
        *pp_head = new_node;
    else
    {
        ListNode *cur = *pp_head;
        // 索引到最后一个节点
        while (cur->next_ != NULL)
        {
            cur = cur->next_;
        }
        // 将新节点连接至尾节点
        cur->next_ = new_node;
    }
}

// 头删数据
void list_pop_front(ListNode **pp_head)
{
    assert(pp_head);
    assert(*pp_head);

    ListNode *first_node = *pp_head;
    ListNode *second_node = first_node->next_;

    free(first_node);
    *pp_head = second_node;
}

// 尾删数据
void list_pop_back(ListNode **pp_head)
{
    assert(pp_head);
    assert(*pp_head);

    ListNode *cur = *pp_head;
    ListNode *pre = NULL;

    // 索引到最后一个节点
    while (cur->next_ != NULL)
    {
        pre = cur;
        cur = cur->next_;
    }

    if (pre != NULL)
        pre->next_ = NULL;
    else
        *pp_head = NULL;

    free(cur);
}

// 销毁链表
void list_destory(ListNode **pp_head)
{
    assert(pp_head);

    ListNode *pre = NULL;
    ListNode *cur = *pp_head;

    while (cur != NULL)
    {
        pre = cur;
        cur = cur->next_;
        free(pre);
    }

    *pp_head = NULL;
}

// 查找数据
ListNode *list_find(ListNode *p_head, list_data_type val)
{
    assert(p_head);

    ListNode *cur = p_head;
    while (cur != NULL)
    {
        if (cur->data_ == val)
            return cur;
        else
            cur = cur->next_;
    }

    return NULL;
}

// pos位置之前插入数据
void list_insert(ListNode **pp_head, ListNode *pos, list_data_type val)
{
    assert(pp_head);
    assert(pos);

    // 头插
    if (pos == *pp_head)
    {
        list_push_front(pp_head, val);
        return;
    }

    ListNode *pre = *pp_head;
    ListNode *cur = (*pp_head)->next_;

    while(cur != NULL)
    {
        if(cur == pos)
        {
            ListNode* new_node = get_list_node(val);
            pre->next_ = new_node;
            new_node->next_ = cur;
            return;
        }

        pre = cur;
        cur = cur->next_;
    }

    // 走到此处表明pos传递错误
    assert(cur);
}

// 删除pos位置数据
void list_erase(ListNode **pp_head, ListNode *pos)
{
    assert(pp_head);
    assert(pos);

    if(pos == *pp_head)
    {
        list_pop_front(pp_head);
        return;
    }

    ListNode* pre = *pp_head;
    ListNode* cur = (*pp_head)->next_;
    while(cur != NULL)
    {
        if(cur == pos)
        {
            pre->next_ = cur->next_;
            free(cur);
            return;
        }

        pre = cur;
        cur = cur->next_;
    }

    assert(cur);
}