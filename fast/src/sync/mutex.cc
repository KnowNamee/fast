#include <atomic>
#include <sync/mutex.h>

#include <sync/futex.h>

namespace fast::sync {

void Mutex::lock() {
    u32 val = 0;
    if (!_atomic.compare_exchange_strong(val, 1, std::memory_order_acq_rel)) {
        if (val != 2) {
            val = _atomic.exchange(2, std::memory_order_acquire);
        }
        while (val != 0) {
            futexWait(_atomic, 2, std::memory_order_relaxed);
            val = _atomic.exchange(2, std::memory_order_acquire);
        }
    }
}

void Mutex::unlock() {
    if (_atomic.exchange(0, std::memory_order_release) == 2) {
        futexWakeOne(futexAddr(_atomic));
    }
}

}
