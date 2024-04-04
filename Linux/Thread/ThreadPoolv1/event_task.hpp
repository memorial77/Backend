#pragma once

#include <iostream>
#include <string>
#include <functional>

// 定义事件处理任务类
class EventTask
{
    // 使用std::function定义事件处理函数类型
    using handler_t = std::function<void(const std::string &)>;

public:
    // 默认构造函数
    EventTask()
    {
    }

    // 带参数的构造函数，接受事件数据和事件处理函数
    EventTask(const std::string &eventData, handler_t handler)
        : _eventData(eventData), _handler(handler)
    {
    }

    // 重载函数调用操作符，执行事件处理任务
    void operator()()
    {
        // 调用事件处理函数处理事件数据
        _handler(_eventData);
    }

private:
    std::string _eventData; // 事件数据
    handler_t _handler;     // 事件处理函数
};

// 事件处理函数示例：输出事件数据到控制台
void printEventData(const std::string &eventData)
{
    std::cout << "Event data: " << eventData << std::endl;
}

// 事件处理函数示例：将事件数据写入日志文件
void logEventData(const std::string &eventData)
{
    std::cout << "Logging event data: " << eventData << std::endl;
}