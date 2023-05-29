#pragma once

#include <deque>
#include <sync/spinlock.h>

namespace fast::stacks {

struct alignas(64) Stack {
    void* top() {
        return (void*)(((char*)&data) + (size - 64));
    }

public:
    inline static const int size = 1024 * 64;

    char data[size];
};

struct Stacks {
    Stack* get();
    void ret(Stack*);

private:
    using Lock = sync::TASSpinLock;

private:
    Lock _lock;
    std::deque<Stack*> _stacks;
};

}