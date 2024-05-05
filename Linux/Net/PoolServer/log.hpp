#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>

#define DEBUG 1
#define INFO 2
#define WARN 3
#define ERROR 4
#define FATAL 5

// ANSI颜色代码
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

// 定义一个静态文件流对象
static std::ofstream logFile("log.txt", std::ios::app);

void log_msg(int level, const std::string& msg) {
    if (!logFile.is_open()) {
        std::cerr << RED << "无法打开日志文件！" << RESET << std::endl;
        return;
    }

    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&time);

    // 使用strftime格式化时间
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S] ", localTime);

    std::string color;
    std::string levelStr;

    switch(level) {
        case DEBUG:
            color = CYAN;
            levelStr = "[DEBUG] ";
            break;
        case INFO:
            color = GREEN;
            levelStr = "[INFO]  ";
            break;
        case WARN:
            color = YELLOW;
            levelStr = "[WARN]  ";
            break;
        case ERROR:
            color = RED;
            levelStr = "[ERROR] ";
            break;
        case FATAL:
            color = MAGENTA;
            levelStr = "[FATAL] ";
            break;
        default:
            color = RESET;
            levelStr = "[UNKNOWN] ";
            break;
    }

    // 输出到终端，时间戳使用蓝色，日志级别使用指定颜色，消息本身使用默认颜色
    std::cout << BLUE << timeStr << color << levelStr << RESET << msg << RESET << std::endl;
    // 写入到文件
    logFile << timeStr << levelStr << msg << std::endl;
}