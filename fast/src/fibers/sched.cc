#include <fibers/sched.h>
#include <fibers/handle.h>
#include <fibers/fiber.h>

namespace fast::fibers {

namespace {

struct YieldAwaiter : public IAwaiter {
  void awaitSuspend(FiberHandle handle) override {
    handle.Schedule();
  }
};

}  // namespace

void yield() {
  YieldAwaiter await;
  Fiber::current()->suspend(&await);
}

void suspend(IAwaiter* awaiter) {
  Fiber::current()->suspend(awaiter);
}

void go(Scheduler& scheduler, Routine routine) {
  auto* fiber = new Fiber(scheduler, std::move(routine));
  fiber->schedule();
}

void go(Task routine) {
  go(*Scheduler::current(), std::move(routine));
}


}