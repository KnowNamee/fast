#pragma once

#include <sync/futex.h>

#include <atomic>

namespace fast::sync {

class Condvar {
 public:
  template <typename Mutex>
  void wait(Mutex& mu) {
    u32 epoch = epoch_.load(std::memory_order_acquire);
    mu.unlock();
    futexWait(epoch_, epoch, std::memory_order_relaxed);
    mu.lock();
  }

  template <typename Mutex, typename Condition>
  void wait(Mutex& mu, Condition condition) {
    while (!condition()) {
        wait(mu);
    }
  }

  void notify_one();
  void notify_all();

 private:
  std::atomic<u32> epoch_ = 0;
};

}