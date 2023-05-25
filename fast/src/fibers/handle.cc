#include <fibers/handle.h>
#include <fibers/fiber.h>

#include <utility>

namespace fast::fibers {

FiberHandle::FiberHandle()
  : FiberHandle(nullptr)
{ }

bool FiberHandle::IsValid() const {
  return fiber_ != nullptr;
}

FiberHandle::FiberHandle(Fiber* fiber)
  : fiber_(fiber)
{ }

Fiber* FiberHandle::Release() {
  return std::exchange(fiber_, nullptr);
}

void FiberHandle::Schedule() {
  Release()->schedule();
}

}
