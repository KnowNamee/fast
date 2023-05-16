#pragma once

#include <atomic>

#include <types.h>

namespace fast::sync {

class WaitGroup {
 public:
  void add(usize count);
  void done();
  void wait();

 private:
  std::atomic<u32> waiters_{0};
  std::atomic<u32> is_any_waiters_{0};
};

}