#pragma once

#include <coro/trampoline.h>

namespace fast::coro {

struct MachineContext {
  void* rsp_;
  void setup(void* stack, ITrampoline* trampoline);
  void switchTo(MachineContext& target);
};

}
