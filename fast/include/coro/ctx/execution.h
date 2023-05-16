#pragma once

#include <coro/trampoline.h>
#include <coro/ctx/machine.h>

namespace fast::coro {

class ExecutionContext : public ITrampoline {
 public:
  void setup(void* stack, ITrampoline* trampoline);
  void switchTo(ExecutionContext& target);
  [[noreturn]] void exitTo(ExecutionContext& target);

 private:
  void run() noexcept override;

 private:
  ITrampoline* _userTrampoline;
  MachineContext _machineCtx;
};

}