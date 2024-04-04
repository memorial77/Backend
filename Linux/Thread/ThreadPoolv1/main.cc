#include <unistd.h>
#include "thread_pool.hpp"
#include "cal_task.hpp"
#include "event_task.hpp"
#include "thread.hpp"

void tsak_cal()
{
    ThreadPool<CalTask>* thread_pool = ThreadPool<CalTask>::get_instance();
    thread_pool->run();

    char op[4] = {'+', '-', '*', '/'};

    while (true)
    {
        int num1 = rand() % 100 + 1;
        int num2 = rand() % 100 + 1;
        int select = rand() % 4;
        char op_ = op[select];
        CalTask task(num1, num2, op_, mymath);
        thread_pool->push(task);
        std::cout << "产生任务:" << num1 << " " << op_ << " " << num2 << " = ? " << std::endl;
        sleep(1);
    }
}

// void task_event()
// {
//     ThreadPool<EventTask> thread_pool;
//     thread_pool.run();

//     while (true)
//     {
//         int num1 = rand() % 2 + 1;
//         if (num1 == 1)
//         {
//             EventTask task("Event 1 occurred", printEventData);
//             thread_pool.push(task);
//         }
//         else
//         {
//             EventTask task("Event 2 occurred", logEventData);
//             thread_pool.push(task);
//         }

//         sleep(1);
//     }
// }

int main()
{
    tsak_cal();
    // task_event();
}
