#pragma once

#include <types.h>

namespace fast::executors {

struct IExecutor {
  virtual ~IExecutor() = default;
  virtual void submit(Task) = 0;
};

}
