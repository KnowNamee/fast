#include <coro/ctx/machine.h>

#include <cstdint>

extern "C" void* setupMachineContext(void* stack, void* trampoline, void* arg);
extern "C" void switchMachineContext(void* from_rsp, void* to_rsp);

namespace fast::coro {

void machineContextTrampoline(void*, void*, void*, void*, void*, void*, void* arg7) {
  ITrampoline* t = (ITrampoline*)arg7;
  t->run();
}

void MachineContext::setup(void* stack, ITrampoline* trampoline) {
  rsp_ = setupMachineContext((void*)((uint64_t)stack + (1024 * 1024 * 2 - 1)), (void*)machineContextTrampoline, trampoline);
}

void MachineContext::switchTo(MachineContext& target) {
    switchMachineContext(&rsp_, &target.rsp_);
} 

}
