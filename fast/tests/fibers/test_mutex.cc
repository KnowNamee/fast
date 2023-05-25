#include <gtest/gtest.h>

#include <executors/thread_pool.h>
#include <fibers/mutex.h>
#include <fibers/sched.h>

using fast::executors::ThreadPool;

using namespace fast;

TEST(TestMutex, JustWorks) {
    ThreadPool scheduler{4};
    scheduler.start();

    fibers::Mutex mutex;
    size_t cs = 0;

    fibers::go(scheduler, [&] {
      for (size_t j = 0; j < 11; ++j) {
        std::lock_guard guard(mutex);
        ++cs;
      }
    });

    scheduler.wait();

    ASSERT_EQ(cs, 11);

    scheduler.stop();
  }

TEST(TestMutex, Counter) {
    ThreadPool scheduler{4};
    scheduler.start();

    fibers::Mutex mutex;
    size_t cs = 0;

    static const size_t kFibers = 10;
    static const size_t kSectionsPerFiber = 1024;

    for (size_t i = 0; i < kFibers; ++i) {
      fibers::go(scheduler, [&] {
        for (size_t j = 0; j < kSectionsPerFiber; ++j) {
          std::lock_guard guard(mutex);
          ++cs;
        }
      });
    }

    scheduler.wait();

    std::cout << "# cs = " << cs
              << " (expected = " << kFibers * kSectionsPerFiber << ")"
              << std::endl;

    ASSERT_EQ(cs, kFibers * kSectionsPerFiber);

    scheduler.stop();
  }


TEST(TestMutex, Stress1) {
  ThreadPool scheduler{4};
  scheduler.start();

  fibers::Mutex mutex;

  int accesses = 0;

  for (size_t i = 0; i < 1e5; ++i) {
    fibers::go(scheduler, [&] {
        mutex.lock();
        accesses++;
        mutex.unlock();
      });
  }

  scheduler.wait();

  ASSERT_EQ(accesses, 1e5);

  scheduler.stop();
}