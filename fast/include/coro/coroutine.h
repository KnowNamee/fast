#pragma once

#include <coro/ctx/execution.h>
#include <stacks/stacks.h>
#include <exception>
#include <types.h>

namespace fast::coro {

class Coroutine : private ITrampoline {
 public:
  explicit Coroutine(Routine routine);

  void resume();
  bool completed() const;
  static void suspend();

 private:
  void run() noexcept override;

  stacks::Stack* _stack;
  Routine _routine;
  Coroutine* _parent;
  bool _isCompleted;
  ExecutionContext _current;
  ExecutionContext _target;
};

}