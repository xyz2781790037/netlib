#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace muduo{
    class noncopyable{
    public:
        noncopyable(noncopyable &) = delete;
        noncopyable operator=(noncopyable &) = delete;
    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };
}
#endif