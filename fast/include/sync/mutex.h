#pragma once

#include <types.h>

#include <atomic>

namespace fast::sync {

class Mutex {
 public:
  void lock();
  void unlock();

 private:
  std::atomic<u32> _atomic = 0;
};

}
