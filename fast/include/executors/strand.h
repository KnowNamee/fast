#pragma once

#include <executors/exe.h>
#include <sync/spinlock.h>
#include <memory>
#include <queue>

namespace fast::executors {

class Strand : public IExecutor {
  using Tasks = std::queue<Task>;
  using TasksPtr = std::shared_ptr<Tasks>;

 public:
  explicit Strand(IExecutor&);
  void submit(Task) override;

 private:
  IExecutor& _decorated;

  sync::TASSpinLock _lock;
  TasksPtr _tasks;
  bool _isRunning;

 private:
  void doWork();
};

}
