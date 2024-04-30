#pragma once

#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include "log.hpp"

class Daemon
{
public:
    static void createDaemon(const char *currPath = nullptr)
    {
        // 忽略SIGPIPE信号，防止因写入到关闭的socket或管道时导致进程退出
        signal(SIGPIPE, SIG_IGN);

        // 第一步：创建子进程，父进程退出
        pid_t pid = fork();
        if (pid < 0)
        {
            log_msg(ERROR, "fork error");
            exit(EXIT_FAILURE);
        }
        if (pid > 0)
            exit(EXIT_SUCCESS);

        // 第二步：创建新会话
        if (setsid() < 0)
            exit(EXIT_FAILURE);

        // 第三步：再次fork，确保进程不是会话首进程
        pid = fork();
        if (pid < 0)
            exit(EXIT_FAILURE);
        if (pid > 0)
            exit(EXIT_SUCCESS);

        // 第四步：更改文件权限掩码
        umask(0);

        // 第五步：更改工作目录
        if (currPath != nullptr)
        {
            if (chdir(currPath) < 0)
                exit(EXIT_FAILURE);
        }
        else
            chdir("/");

        // 第六步：关闭所有打开的文件描述符
        for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
            close(x);

        // 第七步：重定向标准输入、输出和错误输出到/dev/null
        open("/dev/null", O_RDWR); // 标准输入
        dup(0);                    // 标准输出
        dup(0);                    // 标准错误
    }
};
