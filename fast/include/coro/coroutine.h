#pragma once

#include <coro/ctx/execution.h>
#include <exception>
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

  char _stack[1024 * 1024 * 3];
  Routine _routine;
  Coroutine* _parent;
  bool _isCompleted;
  ExecutionContext _current;
  ExecutionContext _target;
};

}