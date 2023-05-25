#include <fibers/fiber.h>

namespace fast::fibers {

thread_local Fiber* f = nullptr;

Fiber::Fiber(Scheduler& scheduler, Routine routine)
    : scheduler_(scheduler)
    , awaiter_(nullptr)
    , coro_(std::move(routine))
{ }

void Fiber::schedule() {
  scheduler_.submit([this] {
    this->run();
  });
}

void Fiber::run() {
  f = this;
  coro_.resume();
  f = nullptr;
  if (!coro_.completed()) {
    awaiter_->awaitSuspend(FiberHandle(this));
    return;
  }
  delete this;
}

Fiber* Fiber::current() {
  return f;
}

void Fiber::suspend(IAwaiter* awaiter) {
  awaiter_ = awaiter;
  coro_.suspend();
}

}
