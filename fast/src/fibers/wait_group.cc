#include <fibers/wait_group.h>

namespace fast::fibers {

WaitGroup::WaitGroup()
  : _waiters(0)
  , _isAnyWaiters(0)
  , _futex(_isAnyWaiters)
{ }

WaitGroup::~WaitGroup() {
  wait();
}

void WaitGroup::add(usize count) {
  _isAnyWaiters.store(1);
  _waiters.fetch_add(count);
}

void WaitGroup::done() {
  u32 old_waiters = _waiters.fetch_sub(1);
  if (old_waiters == 1) {
    _isAnyWaiters.store(1);
    _futex.wakeAll();
    _isAnyWaiters.store(2);
  }
}

void WaitGroup::wait() {
  while (true) {
    _futex.wait(1);
    if (_waiters.load() == 0 && _isAnyWaiters.load() == 2) {
      return;
    }
  }
}

}
