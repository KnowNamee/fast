#include <sync/condvar.h>

namespace fast::sync {

void Condvar::notify_one() {
    epoch_.fetch_add(1, std::memory_order_release);
    futexWake(futexAddr(epoch_), 1);
}

void Condvar::notify_all() {
    epoch_.fetch_add(1, std::memory_order_release);
    futexWake(futexAddr(epoch_), INT32_MAX);
}

}
