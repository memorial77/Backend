#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <functional>

// 定义任务类
class CalTask
{
    // 使用std::function定义回调函数类型
    using func_t = std::function<int(int, int, char)>;

public:
    // 默认构造函数
    CalTask()
    {
    }

    // 带参数的构造函数，接受操作数、运算符和回调函数
    CalTask(int x, int y, char op, func_t func)
        : _x(x), _y(y), _op(op), _callback(func)
    {
    }

    // 重载函数调用操作符，执行任务并返回结果字符串
    std::string operator()()
    {
        // 调用回调函数计算结果
        int result = _callback(_x, _y, _op);
        
        // 格式化输出结果字符串
        char buffer[1024];
        snprintf(buffer, sizeof buffer, "%d %c %d = %d", _x, _op, _y, result);
        return buffer;
    }

    // 返回包含占位符的任务字符串
    std::string toTaskString()
    {
        char buffer[1024];
        snprintf(buffer, sizeof buffer, "%d %c %d = ?", _x, _op, _y);
        return buffer;
    }

private:
    int _x;          // 第一个操作数
    int _y;          // 第二个操作数
    char _op;        // 运算符
    func_t _callback; // 回调函数
};

const std::string oper = "+-*/%"; // 支持的运算符字符串

// 自定义的数学运算函数
int mymath(int x, int y, char op)
{
    int result = 0;
    switch (op)
    {
    case '+':
        result = x + y;
        break;
    case '-':
        result = x - y;
        break;
    case '*':
        result = x * y;
        break;
    case '/':
    {
        if (y == 0)
        {
            std::cerr << "div zero error!" << std::endl;
            result = -1;
        }
        else
            result = x / y;
    }
    break;
    case '%':
    {
        if (y == 0)
        {
            std::cerr << "mod zero error!" << std::endl;
            result = -1;
        }
        else
            result = x % y;
    }
    break;
    default:
        // 默认情况下不进行任何操作
        break;
    }

    return result;
}
