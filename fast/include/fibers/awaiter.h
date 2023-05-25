#pragma once

#include <fibers/handle.h>

namespace fast::fibers {

struct IAwaiter {
  virtual ~IAwaiter() = default;
  virtual void awaitSuspend(FiberHandle handle) = 0;
};

}