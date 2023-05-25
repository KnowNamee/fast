#include <atomic>
#include <fibers/mutex.h>

namespace fast::fibers {

Mutex::Mutex()
  : atomic_(0)
  , fu_(atomic_)
{ }

void Mutex::lock() {
  u32 val = 0;
  if (!atomic_.compare_exchange_strong(val, 1, std::memory_order_acq_rel)) {
    if (val != 2) {
      val = atomic_.exchange(2, std::memory_order_acquire);
    }
    while (val != 0) {
      fu_.wait(2);
      val = atomic_.exchange(2, std::memory_order_acquire);
    }
  }
}

void Mutex::unlock() {
  if (atomic_.exchange(0, std::memory_order_release) == 2) {
    fu_.wakeOne();
  }
}

}
