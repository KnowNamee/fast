#include <sync/condvar.h>

namespace fast::sync {

void Condvar::notify_one() {
    epoch_.fetch_add(1, std::memory_order_release);
    futexWakeOne(futexAddr(epoch_));
}

void Condvar::notify_all() {
    epoch_.fetch_add(1, std::memory_order_release);
    futexWakeAll(futexAddr(epoch_));
}

}
