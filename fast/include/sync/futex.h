#pragma once

#include <types.h>

#include <atomic>

namespace fast::sync {

void futexWait(std::atomic<u32>& atomic, u32 old, std::memory_order mo = std::memory_order_seq_cst);
void futexWake(u32* addr, u32 count);
u32* futexAddr(std::atomic<u32>& atomic);

}
