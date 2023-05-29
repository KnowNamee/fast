#pragma once

#include <executors/exe.h>
#include <queue/mpmcub.h>
#include <thread>


namespace fast::executors {

class ThreadPool : public IExecutor {
  using Worker = std::thread;
  using Workers = std::vector<Worker>;

 public:
  explicit ThreadPool(usize);

  void start();
  void submit(Task) override;
  void wait();
  void stop();

  static ThreadPool* current();

 private:
  usize _workersNumber;

  queue::MPMCUnboundedBlockingQueue<Task> _tasks;
  Workers _workers;

  std::atomic<u32> _waitingTasks{0};
};

}