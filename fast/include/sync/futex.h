#pragma once

#include <types.h>

#include <atomic>

namespace fast::sync {

void futexWait(std::atomic<u32>& atomic, u32 old, std::memory_order mo = std::memory_order_seq_cst);
void futexWakeOne(u32* addr);
void futexWakeAll(u32* addr);
u32* futexAddr(std::atomic<u32>& atomic);

}
