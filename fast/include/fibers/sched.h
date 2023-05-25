#pragma once

#include <executors/thread_pool.h>
#include <fibers/awaiter.h>
#include <types.h>

namespace fast::fibers {

using Scheduler = executors::ThreadPool;

void go(Scheduler& scheduler, Task routine);
void go(Task routine);
void suspend(IAwaiter* awaiter);
void yield();

}
