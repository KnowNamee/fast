#include <fibers/futex.h>
#include <fibers/sched.h>

#include <mutex>

namespace fast::fibers {

Futex::Futex(std::atomic<u32>& cell)
  : _cell(cell)
  , _waiters(0)
{ }

void Futex::wait(u32 old) {
  while (_cell.load() == old) {
    atomicPark(old);
  }
}

void Futex::wakeOne() {
  std::unique_lock guard(_spin);
  if (!_waitQueue.empty()) {
    FiberHandle handle = _waitQueue.front();
    _waitQueue.pop();
    handle.Schedule();
    _waiters.fetch_sub(1);
  }
}

void Futex::wakeAll() {
  std::unique_lock guard(_spin);
  _waiters.fetch_sub(_waitQueue.size());
  while (!_waitQueue.empty()) {
    FiberHandle handle = _waitQueue.front();
    _waitQueue.pop();
    handle.Schedule();
  }
}

Futex::FutexAwaiter::FutexAwaiter(Futex* futex)
  : _futex(futex)
{ }

void Futex::FutexAwaiter::awaitSuspend(FiberHandle handle) {
  _futex->_waitQueue.push(handle);
  _futex->_spin.unlock();
}

void Futex::atomicPark(u32 old) {
  _waiters.fetch_add(1);
  _spin.lock();
  if (_cell.load() == old) {
    FutexAwaiter awaiter(this);
    fibers::suspend(&awaiter);
  } else {
    _waiters.fetch_sub(1);
    _spin.unlock();
  }
}

}
