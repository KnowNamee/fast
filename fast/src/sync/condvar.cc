#include <sync/condvar.h>

namespace fast::sync {

void Condvar::notify_one() {
    auto addr = futexAddr(epoch_);
    epoch_.fetch_add(1, std::memory_order_release);
    futexWakeOne(addr);
}

void Condvar::notify_all() {
    auto addr = futexAddr(epoch_);
    epoch_.fetch_add(1, std::memory_order_release);
    futexWakeAll(addr);
}

}
