#include <atomic>
#include <sync/mutex.h>

#include <sync/futex.h>

namespace fast::sync {

void Mutex::lock() {
    u32 val = 0;
    if (!_atomic.compare_exchange_strong(val, 1, std::memory_order_acquire)) {
        if (val != 2) {
            val = _atomic.exchange(2, std::memory_order_relaxed);
        }
        while (val != 0) {
            futexWait(_atomic, 2, std::memory_order_relaxed);
            val = _atomic.exchange(2, std::memory_order_relaxed);
        }
    }
}

void Mutex::unlock() {
    if (_atomic.fetch_sub(1, std::memory_order_release) != 1) {
        _atomic.store(0, std::memory_order_relaxed);
        futexWake(futexAddr(_atomic), 1);
    }
}

}
