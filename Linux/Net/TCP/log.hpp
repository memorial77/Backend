#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>

#define DEBUG 1
#define INFO 2
#define WARN 3
#define ERROR 4
#define FATAL 5

void log_msg(int level, const std::string& msg) {
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&time);

    // 使用strftime格式化时间
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S] ", localTime);

    switch(level) {
        case DEBUG:
            std::cout << timeStr << "[DEBUG] " << msg << std::endl;
            break;
        case INFO:
            std::cout << timeStr << "[INFO]  " << msg << std::endl;
            break;
        case WARN:
            std::cout << timeStr << "[WARN]  " << msg << std::endl;
            break;
        case ERROR:
            std::cerr << timeStr << "[ERROR] " << msg << std::endl;
            break;
        case FATAL:
            std::cerr << timeStr << "[FATAL] " << msg << std::endl;
            break;
        default:
            std::cerr << timeStr << "[UNKNOWN] " << msg << std::endl;
            break;
    }
}