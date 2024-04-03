#include "thread_pool.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUMBER 2 // 添加线程数量

// 任务结构体
typedef struct Task
{
    void (*function)(void *arg); // 函数指针
    void *arg;                   // 函数参数
} Task;

// 线程池结构体
struct ThreadPool
{
    // 任务队列
    Task *task_queue;
    int queue_cap;   // 队列容量
    int queue_size;  // 当前任务数量
    int queue_front; // 队头取数据
    int queue_rear;  // 队尾放数据

    pthread_t manager_id;  // 管理者线程id
    pthread_t *thread_ids; // 工作线程id数组
    int min_num;           // 最小线程数
    int max_num;           // 最大线程数
    int busy_num;          // 忙碌线程数
    int live_num;          // 存活线程数
    int exit_num;          // 销毁线程数

    pthread_mutex_t mutex_pool; // 线程池锁
    pthread_mutex_t mutex_busy; // 忙碌线程数锁
    pthread_cond_t not_full;    // 任务队列是否为满
    pthread_cond_t not_empty;   // 任务队列是否为空

    int shut_down; // 0-不销毁 1-销毁
};

// 线程池初始化
ThreadPool *thread_pool_create(int cap, int min, int max)
{
    // 堆区创建内线程池对象
    ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool));
    do
    {
        if (pool == NULL)
        {
            printf("malloc pthread_pool fail\n");
            break;
        }

        // 堆区创建线程id数组
        pool->thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * max);
        if (pool->thread_ids == NULL)
        {
            printf("malloc thread_ids fail\n");
            break;
        }
        memset(pool->thread_ids, 0, sizeof(pthread_t) * max);

        pool->min_num = min;
        pool->max_num = max;
        pool->busy_num = 0;
        pool->live_num = min; // 初始化时有最小线程数量
        pool->exit_num = 0;   // 根据实际场景赋值

        // 初始化互斥锁和同步变量
        if (pthread_mutex_init(&pool->mutex_pool, NULL) != 0 ||
            pthread_mutex_init(&pool->mutex_busy, NULL) != 0 ||
            pthread_cond_init(&pool->not_full, NULL) != 0 ||
            pthread_cond_init(&pool->not_empty, NULL) != 0)
        {
            printf("mutex or cond init fail\n");
            break;
        }

        // 任务队列相关属性
        pool->task_queue = (Task *)malloc(sizeof(Task) * cap);
        if (pool->task_queue == NULL)
        {
            printf("malloc task_queue fail\n");
            break;
        }
        pool->queue_cap = cap;
        pool->queue_size = 0;
        pool->queue_front = 0;
        pool->queue_rear = 0;

        pool->shut_down = 0;

        // 管理者线程
        pthread_create(&pool->manager_id, NULL, manager, pool);

        // 创建初始最少数量工作线程
        for (int i = 0; i < pool->min_num; i++)
        {
            pthread_create(&pool->thread_ids[i], NULL, worker, pool);
        }

        return pool;
    } while (0);

    // 若出现异常跳出while语句则进行资源释放
    if (pool && pool->thread_ids)
        free(pool->thread_ids);

    if (pool && pool->task_queue)
        free(pool->task_queue);

    if (pool)
        free(pool);

    return NULL;
}

// 工作线程函数
void *worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *)(arg);

    while (1)
    {
        // 对线程池加锁
        pthread_mutex_lock(&pool->mutex_pool);
        // 判断任务队列是否为空，为空则阻塞
        while (pool->queue_size == 0 && !pool->shut_down)
        {
            pthread_cond_wait(&pool->not_empty, &pool->mutex_pool);

            // 检测是否需要销毁
            if (pool->exit_num > 0)
            {
                pool->exit_num--;
                if (pool->live_num > pool->min_num)
                {
                    pool->live_num--;
                    pthread_mutex_unlock(&pool->mutex_pool);
                    thread_exit(pool);
                }
            }
        }

        // 判断线程池是否关闭
        if (pool->shut_down)
        {
            pthread_mutex_unlock(&pool->mutex_pool);
            thread_exit(pool);
        }

        // 从任务队列取出一个任务
        Task task;
        task.function = pool->task_queue[pool->queue_front].function;
        task.arg = pool->task_queue[pool->queue_front].arg;

        // 移动头节点
        pool->queue_front = (pool->queue_front + 1) % pool->queue_cap;
        // 更新任务队列中任务数量
        pool->queue_size--;

        pthread_cond_signal(&pool->not_full);
        // 解锁
        pthread_mutex_unlock(&pool->mutex_pool);

        printf("thread start working...\n");
        pthread_mutex_lock(&pool->mutex_busy);
        pool->busy_num++;
        pthread_mutex_unlock(&pool->mutex_busy);

        // 执行任务函数
        task.function(task.arg);
        free(task.arg);
        task.arg = NULL;

        pthread_mutex_lock(&pool->mutex_busy);
        pool->busy_num--;
        pthread_mutex_unlock(&pool->mutex_busy);
        printf("thread end working...\n");
    }
}

// 管理者线程函数
void *manager(void *arg)
{
    ThreadPool *pool = (ThreadPool *)(arg);

    while (!pool->shut_down)
    {
        // 每隔三秒检测一次
        sleep(3);
        pthread_mutex_lock(&pool->mutex_pool);   // 对线程池加锁
        int queue_size = pool->queue_size;       // 任务数量
        int live_num = pool->live_num;           // 当前线程数量
        pthread_mutex_unlock(&pool->mutex_pool); // 解锁线程池

        pthread_mutex_lock(&pool->mutex_busy);
        int busy_num = pool->busy_num; // 忙碌线程数量
        pthread_mutex_unlock(&pool->mutex_busy);

        // 添加线程
        // 条件：任务数量>存活线程数量 && 存活线程数量<最大线程数量
        if (queue_size > live_num && live_num < pool->max_num)
        {
            int count = 0;
            pthread_mutex_pock(&pool->mutex_pool);
            for (int i = 0; i < pool->max_num && count < NUMBER && pool->live_num < pool->max_num; i++)
            {
                if (pool->thread_ids[i] == 0)
                {
                    pthread_create(&pool->thread_ids[i], NULL, worker, pool);
                    pool->live_num++;
                    count++;
                }
            }
            pthread_mutex_unlock(&pool->mutex_pool);
        }

        // 销毁线程
        // 条件：忙碌线程数*2 < 存活线程数 && 存活线程数 > 最小线程数
        if (busy_num * 2 < live_num && live_num > pool->min_num)
        {
            pthread_mutex_lock(&pool->mutex_pool);
            pool->exit_num = NUMBER;
            pthread_mutex_unlock(&pool->mutex_pool);

            // 让阻塞的线程自杀
            for (int i = 0; i < NUMBER; i++)
                pthread_cond_signal(&pool->not_empty);
        }
    }
}

// 线程退出函数
void thread_exit(ThreadPool *pool)
{
    pthread_t tid = pthread_self();

    for (int i = 0; i < pool->max_num; i++)
    {
        if (pool->thread_ids[i] == tid)
        {
            pool->thread_ids[i] = 0;
            printf("thread_exit called, %ld is exiting...\n", tid);
            break;
        }
    }
    pthread_exit(NULL);
}

// 添加任务函数
void task_add(ThreadPool *pool, void (*function)(void *), void *arg)
{
    pthread_mutex_lock(&pool->mutex_pool);

    while (pool->queue_size == pool->queue_cap && !pool->shut_down)
    {
        pthread_cond_wait(&pool->not_full, &pool->mutex_pool);
    }

    if (pool->shut_down)
    {
        pthread_mutex_unlock(&pool->mutex_pool);
        return;
    }

    // 添加任务至队列队尾
    pool->task_queue[pool->queue_rear].function = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_cap;
    pool->queue_size++;

    pthread_cond_signal(&pool->not_empty);
    pthread_mutex_unlock(&pool->mutex_pool);
}

// 忙碌线程数量
int busy_threads(ThreadPool *pool)
{
    pthread_mutex_lock(&pool->mutex_busy);
    int busy_threads = pool->busy_num;
    pthread_mutex_unlock(&pool->mutex_busy);
    return busy_threads;
}

// 存活线程数量
int live_threads(ThreadPool *pool)
{
    pthread_mutex_lock(&pool->live_num);
    int live_threads = pool->live_num;
    pthread_mutex_unlock(&pool->live_num);
    return live_threads;
}

// 销毁线程池
int thread_pool_destory(ThreadPool *pool)
{
    if (pool == NULL)
        return -1;

    // 关闭标志位设置为1
    pool->shut_down = 1;
    // 阻塞回收管理者线程
    pthread_join(&pool->manager_id, NULL);
    // 唤醒阻塞的消费者线程
    for (int i = 0; i < pool->live_num; i++)
        pthread_cond_signal(&pool->not_empty);
    // 释放堆内存
    if (pool->task_queue)
        free(pool->task_queue);
    if (pool->thread_ids)
        free(pool->thread_ids);

    pthread_mutex_destroy(&pool->mutex_busy);
    pthread_mutex_destroy(&pool->mutex_pool);
    pthread_cond_destroy(&pool->not_empty);
    pthread_cond_destroy(&pool->not_full);

    free(pool);
    pool = NULL;
    return 0;
}
