#pragma once

#include "logStream.h"
#include "Timestamp.h"
#include "noncopyable.h"

#include <time.h>

#define LOG_TRACE                                          \
    if (mulib::base::Logger::logLevel() <= mulib::base::Logger::TRACE) \
    mulib::base::Logger(__FILE__, __LINE__, mulib::base::Logger::TRACE, __func__).stream()

#define LOG_DEBUG                                          \
    if (mulib::base::Logger::logLevel() <= mulib::base::Logger::DEBUG) \
    mulib::base::Logger(__FILE__, __LINE__, mulib::base::Logger::DEBUG, __func__).stream()

#define LOG_INFO                                          \
    if (mulib::base::Logger::logLevel() <= mulib::base::Logger::INFO) \
    mulib::base::Logger(__FILE__, __LINE__).stream()

#define LOG_WARN mulib::base::Logger(__FILE__, __LINE__, mulib::base::Logger::WARN).stream()
#define LOG_ERROR mulib::base::Logger(__FILE__, __LINE__, mulib::base::Logger::ERROR).stream()
#define LOG_FATAL mulib::base::Logger(__FILE__, __LINE__, mulib::base::Logger::FATAL).stream()

namespace mulib
{
    namespace base
    {
        
        class Logger{
        public:
            enum LogLevel{
                TRACE, // 追踪程序的详细运行过程
                DEBUG, // 调试信息
                INFO,  // 一般性信息，如程序启动、配置加载成功等
                WARN,  // 警告，表示程序出现了轻微异常或潜在问题，但还能运行
                ERROR, // 错误
                FATAL  // 致命错误
            };
            class SourceFile{
            public:
                template <int N>
                SourceFile(const char (&arr)[N]); // 用于记录 __FILE__
                explicit SourceFile(const char *filename);

                const char *data_;
                int size_;
            };
            Logger(SourceFile file, int line);
            Logger(SourceFile file, int line, LogLevel level);
            Logger(SourceFile file, int line, LogLevel level, const char *func);
            Logger(SourceFile file, int line, bool toAbort); // SYSFATAL 日志

            ~Logger();

            LogStream &stream(); // 获取流式日志输入

            static LogLevel logLevel();
            static void setLogLevel(LogLevel level);

            typedef void (*OutputFunc)(const char *msg, int len);
            typedef void (*FlushFunc)();
            static void setOutput(OutputFunc);
            static void setFlush(FlushFunc);
        private:
            class Impl{
            public:
                Impl(LogLevel level, int savedErrno, const SourceFile &file, int line);
                void formatTime();
                void finish();

                mulib::base::Timestamp time_;
                LogStream stream_;
                LogLevel level_;
                int line_;
                SourceFile basename_;
            };

            Impl impl_;
        };
    }
}