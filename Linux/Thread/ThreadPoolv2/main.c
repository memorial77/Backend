#include "thread_pool.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define TASK_NUM 100

void task_fun(void *arg)
{
    int num = *((int *)arg);
    printf("thread is working, number = %d, id = %ld\n", num, pthread_self());
    usleep(1000);
}

int main()
{
    ThreadPool *pool = pthread_pool_create(TASK_NUM, 3, 10);

    for (int i = 0; i < TASK_NUM; i++)
    {
        int *num = (int *)malloc(sizeof(int));
        *num = i + 100;
        task_add(pool, task_fun, num);
    }

    sleep(30);
    thread_pool_destory(pool);
    return 0;
}