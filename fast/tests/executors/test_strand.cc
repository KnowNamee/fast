#include <gtest/gtest.h>

#include <chrono>

#include <executors/strand.h>
#include <executors/thread_pool.h>
#include <executors/manual.h>
#include <executors/exe.h>

using fast::executors::Strand;
using fast::executors::ThreadPool;
using fast::executors::IExecutor;
using fast::executors::ManualExecutor;

using namespace std::chrono_literals;

void AssertRunningOn(ThreadPool& pool) {
  ASSERT_TRUE(ThreadPool::current() == &pool);
}

TEST(TestStrand, JustWorks) {
    ThreadPool pool{4};
    pool.start();

    Strand strand{pool};

    bool done = false;

    strand.submit([&]{
      done = true;
    });

    pool.wait();

    ASSERT_TRUE(done);

    pool.stop();
  }

TEST(TestStrand, Decorator) {
    ThreadPool pool{4};
    pool.start();

    Strand strand{pool};

    bool done{false};

    for (size_t i = 0; i < 128; ++i) {
      strand.submit([&]{
        AssertRunningOn(pool);
        done = true;
      });
    }

    pool.wait();

    ASSERT_TRUE(done);

    pool.stop();
  }

TEST(TestStrand, Counter) {
    ThreadPool pool{13};
    pool.start();

    size_t counter = 0;

    Strand strand{pool};

    static const size_t kIncrements = 1234;

    for (size_t i = 0; i < kIncrements; ++i) {
      strand.submit([&]{
        AssertRunningOn(pool);
        ++counter;
      });
    };

    pool.wait();

    ASSERT_EQ(counter, kIncrements);

    pool.stop();
  }

TEST(TestStrand, Fifo) {
    ThreadPool pool{13};
    pool.start();

    Strand strand{pool};

    size_t done = 0;

    static const size_t kTasks = 12345;

    for (size_t i = 0; i < kTasks; ++i) {
      strand.submit([&, i=i]{
        AssertRunningOn(pool);
        ASSERT_EQ(done++, i);
      });
    };

    pool.wait();

    ASSERT_EQ(done, kTasks);

    pool.stop();
  }

  class Robot {
   public:
    explicit Robot(IExecutor& executor)
        : strand_(executor) {
    }

    void Push() {
      strand_.submit([&] {
        ++steps_;
      });
    }

    size_t Steps() const {
      return steps_;
    }

   private:
    Strand strand_;
    size_t steps_{0};
};

TEST(TestStrand, ConcurrentStrands) {
    ThreadPool pool{16};
    pool.start();

    static const size_t kStrands = 50;

    std::deque<Robot> robots;
    for (size_t i = 0; i < kStrands; ++i) {
      robots.emplace_back(pool);
    }

    static const size_t kPushes = 25;
    static const size_t kIterations = 25;

    for (size_t i = 0; i < kIterations; ++i) {
      for (size_t j = 0; j < kStrands; ++j) {
        for (size_t k = 0; k < kPushes; ++k) {
          robots[j].Push();
        }
      }
    }

    pool.wait();

    for (size_t i = 0; i < kStrands; ++i) {
      ASSERT_EQ(robots[i].Steps(), kPushes * kIterations);
    }

    pool.stop();
}

TEST(TestStrand, ConcurrentSubmits) {
    ThreadPool workers{2};
    Strand strand{workers};

    ThreadPool clients{4};

    workers.start();
    clients.start();

    static const size_t kTasks = 1024;

    size_t task_count{0};

    for (size_t i = 0; i < kTasks; ++i) {
      clients.submit([&] {
        strand.submit([&] {
          AssertRunningOn(workers);
          ++task_count;
        });
      });
    }

    clients.wait();
    workers.wait();

    ASSERT_EQ(task_count, kTasks);

    workers.stop();
    clients.stop();
}

TEST(TestStrand, StrandOverManual) {
    ManualExecutor manual;
    Strand strand{manual};

    static const size_t kTasks = 117;

    size_t tasks = 0;

    for (size_t i = 0; i < kTasks; ++i) {
      strand.submit([&] {
        ++tasks;
      });
    }

    manual.drain();

    ASSERT_EQ(tasks, kTasks);
}

TEST(TestStrand, Batching) {
    ManualExecutor manual;
    Strand strand{manual};

    static const size_t kTasks = 100;

    size_t completed = 0;
    for (size_t i = 0; i < kTasks; ++i) {
      strand.submit([&completed] {
        ++completed;
      });
    };

    size_t tasks = manual.drain();
    ASSERT_LT(tasks, 5);
}

TEST(TestStrand, StrandOverStrand) {
    ThreadPool pool{4};
    pool.start();

    Strand strand_1{pool};
    Strand strand_2{(IExecutor&)strand_1};

    static const size_t kTasks = 17;

    size_t tasks = 0;

    for (size_t i = 0; i < kTasks; ++i) {
      strand_2.submit([&tasks] {
        ++tasks;
      });
    }

    pool.wait();

    ASSERT_EQ(tasks, kTasks);

    pool.stop();
}

TEST(TestStrand, DoNotOccupyThread) {
    ThreadPool pool{1};
    pool.start();

    Strand strand{pool};

    pool.submit([] {
      std::this_thread::sleep_for(1s);
    });

    std::atomic<bool> stop_requested{false};

    auto snooze = []() {
      std::this_thread::sleep_for(10ms);
    };

    for (size_t i = 0; i < 100; ++i) {
      strand.submit(snooze);
    }

    pool.submit([&stop_requested] {
      stop_requested.store(true);
    });

    while (!stop_requested.load()) {
      strand.submit(snooze);
      std::this_thread::sleep_for(10ms);
    }

    pool.wait();
    pool.stop();
}
