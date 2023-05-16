#include <executors/manual.h>

namespace fast::executors {

void ManualExecutor::submit(Task task) {
  _tasks.push(std::move(task));
}

usize ManualExecutor::runAtMost(usize limit) {
  usize counter = 0;
  for (usize i = 0; i < limit; ++i) {
    if (_tasks.empty()) {
      break;
    }
    auto task = std::move(_tasks.front());
    _tasks.pop();
    task();
    counter++;
  }
  return counter;
}

usize ManualExecutor::drain() {
  usize counter = 0;
  while (!_tasks.empty()) {
    Task task = std::move(_tasks.front());
    _tasks.pop();
    task();
    counter++;
  }
  return counter;
}

bool ManualExecutor::runNext() {
  return runAtMost(1) == 1;
}

usize ManualExecutor::taskCount() const {
  return _tasks.size();
}

bool ManualExecutor::isEmpty() const {
  return _tasks.empty();
}

bool ManualExecutor::nonEmpty() const {
  return !isEmpty();
}

}
