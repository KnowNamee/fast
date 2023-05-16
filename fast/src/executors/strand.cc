#include "sync/spinlock.h"
#include <executors/strand.h>

#include <mutex>

namespace fast::executors {

Strand::Strand(IExecutor& decorated)
    : _decorated(decorated)
    , _isRunning(false)
{ }

void Strand::submit(Task task) {
  std::lock_guard<sync::TASSpinLock> guard(_lock);
  _tasks->push(std::move(task));
  if (_isRunning) {
    return;
  }
  _isRunning = true;
  _decorated.submit([this] {
    this->doWork();
  });
}

void Strand::doWork() {
  TasksPtr next_queue = std::make_shared<Tasks>();
  TasksPtr prev_queue;

  {
    std::lock_guard<sync::TASSpinLock> guard(_lock);
    prev_queue = _tasks;
    _tasks = next_queue;
  }

  usize size = prev_queue->size();
  std::vector<Task> tasks;
  tasks.reserve(size);
  for (usize i = 0; i < size; ++i) {
    tasks.push_back(std::move(prev_queue->front()));
    prev_queue->pop();
    tasks.back()();
  }

  std::lock_guard<sync::TASSpinLock> guard(_lock);
  if (!next_queue->empty()) {
    _decorated.submit([this] {
      this->doWork();
    });
  } else {
    _isRunning = false;
  }
}

}
