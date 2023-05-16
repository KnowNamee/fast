#include <coro/coroutine.h>

namespace fast::coro {

thread_local Coroutine* coro = nullptr;

Coroutine::Coroutine(Routine routine)
  : _routine(std::move(routine))
  , _parent(coro)
  , _ex(nullptr)
{
  _target.setup(&_stack, this);
}

void Coroutine::resume() {
  coro = this;
  _current.switchTo(_target);
  coro = _parent;
  if (_ex != nullptr) {
    std::rethrow_exception(_ex);
  }
}

void Coroutine::suspend() {
  coro->_target.switchTo(coro->_current);
}

bool Coroutine::completed() const {
  return _isCompleted;
}

void Coroutine::run() noexcept {
  try {
    _routine();
  } catch (...) {
    _ex = std::current_exception();
  }
  _isCompleted = true;
  _target.exitTo(_current);
}

}
