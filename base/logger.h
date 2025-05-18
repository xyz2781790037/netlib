#pragma once

#include "logStream.h"
#include "noncopyable.h"
#include <time.h>
namespace muduo
{
    namespace log
    {
        enum LogLevel
        {
            TRACE, // 追踪程序的详细运行过程
            DEBUG, // 调试信息
            INFO,  // 一般性信息，如程序启动、配置加载成功等
            WARN,  // 警告，表示程序出现了轻微异常或潜在问题，但还能运行
            ERROR, // 错误
            FATAL  // 致命错误
        };
        class Logger : noncopyable
        {
        public:
            Logger(const char *file, int line, LogLevel level);
            ~Logger();

            LogStream &stream();

            static void setLogLevel(LogLevel level);
            static LogLevel logLevel();

        private:
            void finish();            // 构造完整日志行
            std::string formatTime(); // 返回时间字符串

            LogLevel level_;
            const char *file_;
            int line_;
            LogStream stream_;
        };

    }
}
