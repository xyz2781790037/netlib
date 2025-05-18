#include <iostream>
#include <sstream>
#define MAX_STRING_SIZE 4096
namespace muduo{
    namespace log{
        class LogStream{
        public:
            template <typename T>
            LogStream &operator<<(const T &val);
            LogStream &operator<<(const bool &val);
            LogStream &operator<<(std::ostream &(*manip)(std::ostream &));

            void append(const std::string &s, size_t pos, int n);
            void append(const std::string &s);

            const std::string &str() const;
            void reset();

        private:
            bool isMaxString(){
                if (buffer_.size() < MAX_STRING_SIZE)
                {
                    return true;
                }
                std::cout << "buffer_'size reaching the maximum" << std::endl;
                return false;
            }
            std::string buffer_;
            
        };
    }
}
using namespace muduo::log;
template <typename T>
LogStream &LogStream::operator<<(const T &val)
{
    std::ostringstream oss;
    oss << val;
    if (isMaxString())
    {
        buffer_ += oss.str();
    }
    return *this;
}
LogStream &LogStream::operator<<(const bool &val)
{
    if (isMaxString())
    {
        buffer_ += val ? "true" : "false";
    }
    return *this;
}
LogStream &LogStream::operator<<(std::ostream &(*manip)(std::ostream &))
{
    std::ostringstream oss;
    oss << manip; // 这可以捕获 std::endl
    buffer_ += oss.str();
    return *this;
}
const std::string &LogStream::str() const
{
    return buffer_;
}
void LogStream::reset()
{
    buffer_.clear();
}
void LogStream::append(const std::string &s, size_t pos, int n)
{
    if (isMaxString())
        buffer_.append(s, pos, n);
}
void LogStream::append(const std::string &s)
{
    if (isMaxString())
        buffer_ += s;
}
