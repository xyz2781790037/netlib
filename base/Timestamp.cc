#include "Timestamp.h"
using namespace muduo::base;
void Timestamp::swap(Timestamp &that){
    std::swap(that.microSecondsSinceEpoch_, this->microSecondsSinceEpoch_);
}
bool Timestamp::valid() const { 
    return microSecondsSinceEpoch_ > 0; 
}
int64_t Timestamp::microSecondsSinceEpoch() const{
    return this->microSecondsSinceEpoch_;
}
time_t Timestamp::secondsSinceEpoch() const{
    return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
}
std::string Timestamp::toFormattedString(bool showMicroseconds = true) const{
    if(!showMicroseconds){
        return "0";
    }
    time_t seconds = secondsSinceEpoch();
    tm *localTime = localtime(&seconds);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localTime);
    std::string timeString = std::to_string(this->microSecondsSinceEpoch_);
    timeString = timeString.substr(timeString.size() - 6);
    std::string FormattedString(buf);
}