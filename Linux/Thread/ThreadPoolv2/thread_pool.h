#pragma once

typedef struct ThreadPool ThreadPool;

// 线程池初始化
ThreadPool *thread_pool_create(int cap, int min, int max);

// 忙碌线程数量
int busy_threads(ThreadPool *pool);

// 存活线程数量
int live_threads(ThreadPool *pool);

// 工作线程函数
void *worker(void *arg);

// 添加任务函数
void task_add(ThreadPool *pool, void (*function)(void *), void *arg);

// 管理者线程函数
void *manager(void *arg);

// 线程退出函数
void thread_exit(ThreadPool *pool);

// 销毁线程池
int thread_pool_destory(ThreadPool* pool);