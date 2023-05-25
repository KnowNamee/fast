#include <gtest/gtest.h>

#include <executors/thread_pool.h>
#include <fibers/wait_group.h>
#include <fibers/sched.h>

#include <chrono>

using namespace fast;
using namespace std::chrono_literals;

TEST(TestWaitGroup, OneWaiter) {
    executors::ThreadPool scheduler{5};
    scheduler.start();

    fibers::WaitGroup wg;
    std::atomic<size_t> work{0};
    bool ok = false;

    static const size_t kWorkers = 3;

    wg.add(kWorkers);

    fibers::go(scheduler, [&] {
      wg.wait();
      ASSERT_EQ(work.load(), kWorkers);
      ok = true;
    });

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::go(scheduler, [&] {
        std::this_thread::sleep_for(1s);
        ++work;
        wg.done();
      });
    }

    scheduler.wait();

    ASSERT_TRUE(ok);

    scheduler.stop();
  }

TEST(TestWaitGroup, MultipleWaiters) {
    executors::ThreadPool scheduler{/*threads=*/5};
    scheduler.start();

    fibers::WaitGroup wg;

    std::atomic<size_t> work{0};
    std::atomic<size_t> acks{0};

    static const size_t kWorkers = 3;
    static const size_t kWaiters = 4;

    wg.add(kWorkers);

    for (size_t i = 0; i < kWaiters; ++i) {
      fibers::go(scheduler, [&] {
        wg.wait();
        ASSERT_EQ(work.load(), kWorkers);
        ++acks;
      });
    }

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::go(scheduler, [&] {
        std::this_thread::sleep_for(1s);
        ++work;
        wg.done();
      });
    }

    scheduler.wait();

    ASSERT_EQ(acks.load(), kWaiters);

    scheduler.stop();
  }

TEST(TestWaitGroup, DoNotBlockThread) {
    executors::ThreadPool scheduler{1};
    scheduler.start();

    fibers::WaitGroup wg;
    bool ok = false;

    fibers::go(scheduler, [&] {
      wg.wait();
      ok = true;
    });

    wg.add(1);

    fibers::go(scheduler, [&] {
      for (size_t i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(32ms);
        fibers::yield();
      }
      wg.done();
    });

    scheduler.wait();

    ASSERT_TRUE(ok);

    scheduler.stop();
  }

TEST(TestWaitGroup, Stress1) {
    executors::ThreadPool scheduler{4};
  scheduler.start();

  for (size_t i = 0; i < 1e3; ++i) {
    size_t workers = 5;
    size_t waiters = 10;

    fibers::WaitGroup wg;

    std::atomic<size_t> work{0};
    std::atomic<size_t> acks{0};

    wg.add(workers);

    // Waiters

    for (size_t i = 0; i < waiters; ++i) {
      fibers::go(scheduler, [&] {
        wg.wait();
        ASSERT_EQ(work.load(), workers);
        acks.fetch_add(1);
      });
    }

    // Workers

    for (size_t j = 0; j < workers; ++j) {
      fibers::go(scheduler, [&] {
        work.fetch_add(1);
        wg.done();
      });
    }

    scheduler.wait();

    ASSERT_EQ(acks.load(), waiters);
  }

  scheduler.stop();
}