#include <coro/ctx/execution.h>

#include <cstdlib>

namespace fast::coro {

void ExecutionContext::setup(void* stack, ITrampoline* trampoline) {
  _userTrampoline = trampoline;
  _machineCtx.setup(stack, this);
}

void ExecutionContext::switchTo(ExecutionContext& target) {
  _machineCtx.switchTo(target._machineCtx);
}

void ExecutionContext::exitTo(ExecutionContext& target) {
  _machineCtx.switchTo(target._machineCtx);
  std::abort();
}

void ExecutionContext::run() noexcept {
  _userTrampoline->run();
}

}