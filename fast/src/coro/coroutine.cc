#include <coro/coroutine.h>

#include <iostream>

namespace fast::coro {

namespace {

thread_local Coroutine* coro = nullptr;

}

Coroutine::Coroutine(Routine routine)
  : _routine(std::move(routine))
  , _parent(coro)
  , _isCompleted(false)
{
  _target.setup(&_stack, this);
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
  _target.exitTo(_current);
}

}
