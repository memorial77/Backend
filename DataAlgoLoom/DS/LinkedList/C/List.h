#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int list_data_type;
typedef struct ListNode
{
    list_data_type data_;
    struct ListNode *next_;
} ListNode;

// 打印链表
void list_print(ListNode *p_head);

// 获取新节点
ListNode *get_list_node(list_data_type val);

// 头插数据
void list_push_front(ListNode **p_head, list_data_type val);

// 尾插数据
void list_push_back(ListNode **pp_head, list_data_type val);

// 头删数据
void list_pop_front(ListNode **pp_head);

// 尾删数据
void list_pop_back(ListNode **pp_head);

// 销毁链表
void list_destory(ListNode **pp_head);

// 查找数据
ListNode *list_find(ListNode *p_head, list_data_type val);

// pos位置之前插入数据
void list_insert(ListNode **pp_head, ListNode *pos, list_data_type val);

// 删除pos位置数据
void list_erase(ListNode **pp_head, ListNode *pos);
