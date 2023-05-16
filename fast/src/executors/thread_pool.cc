#include <atomic>
#include <executors/thread_pool.h>
#include <sync/futex.h>

namespace fast::executors {

thread_local ThreadPool* pool = nullptr;

ThreadPool::ThreadPool(usize workersNumber)
	: _workersNumber(workersNumber)
{ }

void ThreadPool::start() {
  for (usize i = 0; i < _workersNumber; ++i) {
    _workers.emplace_back([&] {
      pool = this;
      while (auto task = _tasks.Take()) {
        if (!task) {
          return;
        }
        (*task)();
        if (_waitingTasks.fetch_sub(1, std::memory_order_release) == 1) {
          sync::futexWakeAll(sync::futexAddr(_waitingTasks));
        }
      }
    });
  }
}

void ThreadPool::submit(Task task) {
  _waitingTasks.fetch_add(1);
  _tasks.Put(std::move(task));
}

ThreadPool* ThreadPool::current() {
  return pool;
}

void ThreadPool::wait() {
  while (u32 value = _waitingTasks.load(std::memory_order_acquire)) {
    sync::futexWait(_waitingTasks, value);
  }
}

void ThreadPool::stop() {
  _tasks.Close();
  for (auto& worker : _workers) {
    worker.join();
  }
}

}