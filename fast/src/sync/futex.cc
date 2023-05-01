#pragma once

#include <sync/futex.h>

#include <atomic>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>

namespace fast::sync {

namespace {

int futex(int *uaddr, int futex_op, int val,
          const struct timespec *timeout,
          int *uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

}

// Unix Only ///////////////////////////////////////////////////////////////////////////

void futexWait(std::atomic<u32>& atomic, u32 old, std::memory_order mo) {
    while (atomic.load(mo) == old) {
        futex((int*)futexAddr(atomic), FUTEX_WAIT_PRIVATE, (int)old, nullptr, nullptr, 0);
    }
}

void futexWake(u32* addr, u32 count) {
    futex((int*)addr, FUTEX_WAKE_PRIVATE, (int)count, nullptr, nullptr, 0);
}

u32* futexAddr(std::atomic<u32>& atomic) {
    return (u32*)&atomic;
}

////////////////////////////////////////////////////////////////////////////////////////

}
