#include <stacks/stacks.h>
#include <coro/coroutine.h>

namespace fast::coro {

namespace {

thread_local Coroutine* coro = nullptr;
static stacks::Stacks stacks;

}

Coroutine::Coroutine(Routine routine)
  : _stack(stacks.get())
  , _routine(std::move(routine))
  , _parent(coro)
  , _isCompleted(false)
{
  _target.setup(_stack->top(), this);
}

void Coroutine::resume() {
  coro = this;
  _current.switchTo(_target);
  coro = _parent;
}

void Coroutine::suspend() {
  coro->_target.switchTo(coro->_current);
}

bool Coroutine::completed() const {
  return _isCompleted;
}

void Coroutine::run() noexcept {
  _routine();
  _isCompleted = true;
  stacks.ret(_stack);
  _stack = nullptr;
  _target.exitTo(_current);
}

}
