#pragma once

#include <fibers/futex.h>

namespace fast::fibers {

class Mutex {
 public:
  Mutex();

  void lock();
  void unlock();

 private:
  std::atomic<u32> atomic_;
  Futex fu_;
};

}
