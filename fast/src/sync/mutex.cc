#include <sync/mutex.h>

#include <sync/futex.h>

namespace fast::sync {

void Mutex::lock() {
    u32 val = 0;
    if (!_atomic.compare_exchange_strong(val, 1)) {
        if (val != 2) {
            val = _atomic.exchange(2);
        }
        while (val != 0) {
            futexWait(_atomic, 2);
            val = _atomic.exchange(2);
        }
    }
}

void Mutex::unlock() {
    if (_atomic.fetch_sub(1) != 1) {
        futexWake(_atomic, 1);
    }
}

}
