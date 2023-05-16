#pragma once 

#include <executors/exe.h>
#include <queue>

namespace fast::executors {

class ManualExecutor : public IExecutor {
 public:
  void submit(Task) override;

  usize runAtMost(usize limit);
  bool runNext();
  usize drain();

  usize taskCount() const;
  bool isEmpty() const;
  bool nonEmpty() const;

 private:
  std::queue<Task> _tasks;
};

}
