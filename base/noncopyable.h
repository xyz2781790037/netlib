#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace mulib{
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