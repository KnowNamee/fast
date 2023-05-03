#include <sync/wait_group.h>

#include <sync/futex.h>

namespace fast::sync {

void WaitGroup::add(usize count) {
  is_any_waiters_.store(1);
  waiters_.fetch_add(count);
}

void WaitGroup::done() {
  u32 old_waiters = waiters_.fetch_sub(1);
  if (old_waiters == 1) {
    is_any_waiters_.store(0);
    futexWake(futexAddr(is_any_waiters_), UINT32_MAX);
  }
}

void WaitGroup::wait() {
  if (waiters_.load() == 0 && is_any_waiters_.load() == 0) {
    return;
  }
  futexWait(is_any_waiters_, 1);
}

}
