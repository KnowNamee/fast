#pragma once

#include <coro/ctx/execution.h>
#include <types.h>

namespace fast::coro {

class Coroutine : private ITrampoline {
  using Routine = Task;

 public:
  explicit Coroutine(Routine routine);

  void resume();
  bool completed() const;
  static void suspend();

 private:
  void run() noexcept override;

  Routine _routine;

  Coroutine* _parent;
  bool _isCompleted;
  char _stack[1024 * 1024 * 2];
  ExecutionContext _current;
  ExecutionContext _target;
  std::exception_ptr _ex;
};

}