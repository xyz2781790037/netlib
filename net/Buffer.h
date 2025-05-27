#ifndef MUDUO_NET_BUFFER_H
#define MUDUO_NET_BUFFER_H

#include <vector>
#include <string>
#include <assert.h>

namespace mulib{
    namespace net{
        class Buffer{
        public:
            const static size_t kCheapPrepend = 8;
            const static size_t kInitialSize = 1024;
            Buffer();
            size_t readableBytes() const;
            size_t writableBytes() const;
            size_t prependableBytes() const;

            void swap(Buffer &rhs);
            const char *peek() const;

            void retrieve(size_t len);
            void retrieveUntil(const char *end);
            void retrieveAll();

            std::string retrieveAllAsString();
            std::string retrieveAsString(size_t len);

            void append(const char* data,size_t len);
            void ensureWritableBytes(size_t len);

            char* beginWrite();
            const char *beginWrite() const;
            void hasWritten(size_t len);

            void prepend(const void *data, size_t len);
            size_t internalCapacity() const;
        };
    }
}

#endif