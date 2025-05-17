#include "Timestamp.h"
using namespace muduo::base;
Timestamp Timestamp::now(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}
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
    return FormattedString + "." + timeString;
}
Timestamp Timestamp::addTime(Timestamp timestamp, double seconds){
    int64_t delta = static_cast<int64_t>(seconds * kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}
double Timestamp::timeDifference(Timestamp high, Timestamp low){
    int64_t timedifference = high.microSecondsSinceEpoch_ - low.microSecondsSinceEpoch_;
    double delta = static_cast<double>(timedifference / kMicroSecondsPerSecond);
    return delta;
}
bool Timestamp::operator==(const Timestamp that) const{
    return this->microSecondsSinceEpoch_ == that.microSecondsSinceEpoch_;
}
bool Timestamp::operator<(const Timestamp that) const
{
    return this->microSecondsSinceEpoch_ < that.microSecondsSinceEpoch_;
}
bool Timestamp::operator>(const Timestamp that) const
{
    return this->microSecondsSinceEpoch_ > that.microSecondsSinceEpoch_;
}