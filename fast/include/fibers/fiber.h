#pragma once

#include <executors/thread_pool.h>
#include <coro/coroutine.h>
#include <fibers/awaiter.h>

namespace fast::fibers {

class Fiber {
  using Scheduler = executors::ThreadPool;
  using Routine = Task;

 public:
  explicit Fiber(Scheduler& scheduler, Routine routine);

  void schedule();
  void run();

  void suspend(IAwaiter* awaiter);

  static Fiber* current();

 private:
  Scheduler& scheduler_;
  IAwaiter* awaiter_;
  coro::Coroutine coro_;
};

}
