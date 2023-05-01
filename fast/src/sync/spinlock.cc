#include <sync/spinlock.h>

#include <thread>

namespace fast::sync {

void TASSpinLock::unlock() {
    while (true) {
        if (!_atomic.exchange(1, std::memory_order_acquire)) {
            return;
        }
        while (_atomic.load(std::memory_order_relaxed)) {
            std::this_thread::yield();
        }
    }
}

void TASSpinLock::lock() {
    _atomic.exchange(0, std::memory_order_release);
}

}
