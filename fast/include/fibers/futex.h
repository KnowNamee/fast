#pragma once

#include <atomic>
#include <queue>

#include <sync/spinlock.h>
#include <fibers/handle.h>
#include <fibers/awaiter.h>
#include <types.h>

namespace fast::fibers {

class Futex {
 public:
  explicit Futex(std::atomic<u32>& cell);

  void wait(u32 old);
  void wakeOne();
  void wakeAll();

 private:
  class FutexAwaiter : public IAwaiter {
   public:
    FutexAwaiter(Futex* futex);
    void awaitSuspend(FiberHandle handle) override;

   private:
    Futex* _futex;
  };

 private:
  void atomicPark(u32 old);

 private:
  sync::TASSpinLock _spin;

  std::atomic<uint32_t>& _cell;
  std::atomic<uint32_t> _waiters;
  std::queue<FiberHandle> _waitQueue;
};

}
