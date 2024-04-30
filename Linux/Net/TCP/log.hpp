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

// 定义一个静态文件流对象
static std::ofstream logFile("log.txt", std::ios::app);

void log_msg(int level, const std::string& msg) {
    if (!logFile.is_open()) {
        std::cerr << "无法打开日志文件！" << std::endl;
        return;
    }

    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&time);

    // 使用strftime格式化时间
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S] ", localTime);

    switch(level) {
        case DEBUG:
            logFile << timeStr << "[DEBUG] " << msg << std::endl;
            break;
        case INFO:
            logFile << timeStr << "[INFO]  " << msg << std::endl;
            break;
        case WARN:
            logFile << timeStr << "[WARN]  " << msg << std::endl;
            break;
        case ERROR:
            logFile << timeStr << "[ERROR] " << msg << std::endl;
            break;
        case FATAL:
            logFile << timeStr << "[FATAL] " << msg << std::endl;
            break;
        default:
            logFile << timeStr << "[UNKNOWN] " << msg << std::endl;
            break;
    }
} 