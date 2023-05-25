#pragma once

#include <fibers/futex.h>

namespace fast::fibers {

class WaitGroup {
 public:
  WaitGroup();
  ~WaitGroup();

  void add(usize count);
  void done();
  void wait();

 private:
  std::atomic<u32> _waiters{0};
  std::atomic<u32> _isAnyWaiters{0};
  Futex _futex;
};

}  // namespace exe::fibers
