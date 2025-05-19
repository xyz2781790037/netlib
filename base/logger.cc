#include "logger.h"
using namespace mulib::base;
template <int N>
Logger::SourceFile::SourceFile(const char (&arr)[N]) : data_(arr), size_(N - 1) // 用于接收固定长度数组的引用
{
    const char *slash = strrchr(data_, '/');
    if (slash)
    {
        data_ = slash + 1;
        size_ -= static_cast<int>(data_ - arr);
    }
}
Logger::SourceFile::SourceFile(const char *filename) : data_(filename){
    const char *slash = strrchr(filename, '/');
    if(slash){
        data_ = slash + 1;
    }
    size_ = static_cast<int>(strlen(data_));
}
Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile &file, int line) : time_(Timestamp::now()),  stream_(),
level_(level),  line_(line),  basename_(file)
{
    formatTime();
    formatLevel();
    if(savedErrno != 0){
        stream_ << strerror(savedErrno) << " (errno=" << savedErrno << ") ";
    }
}
void Logger::Impl::formatTime(){
    stream_ << time_.toFormattedString() << " ";
}
void Logger::Impl::formatLevel()
{
    switch (level_)
    {
    case TRACE:
        stream_ << "[TRACE] ";
        break;
    case DEBUG:
        stream_ << "[DEBUG] ";
        break;
    case INFO:
        stream_ << "[INFO ] ";
        break;
    case WARN:
        stream_ << "[WARN ] ";
        break;
    case ERROR:
        stream_ << "[ERROR] ";
        break;
    case FATAL:
        stream_ << "[FATAL] ";
        break;
    default:
        stream_ << "[UNKWN] ";
        break;
    }
}
void Logger::Impl::finish(){
    stream_ << "-" << basename_.data_ << ':' << line_ << '\n';
}
Logger::Logger(SourceFile file, int line) : impl_(INFO, 0, file, line)
{}
Logger::Logger(SourceFile file, int line, LogLevel level) : impl_(level, 0, file, line)
{}
Logger::Logger(SourceFile file, int line, LogLevel level, const char *func) : impl_(level, 0, file, line)
{
    impl_.stream_ << func << " ";
}
Logger::Logger(SourceFile file, int line, bool toAbort) : impl_(toAbort ? FATAL : ERROR, errno, file, line)
{}
LogStream &Logger::stream(){
    return impl_.stream_;
}
Logger::LogLevel g_logLevel = Logger::INFO;

Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}
void Logger::setLogLevel(Logger::LogLevel level)
{
    g_logLevel = level;
}