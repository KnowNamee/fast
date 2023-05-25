#include <gtest/gtest.h>

#include <executors/thread_pool.h>
#include <fibers/sched.h>

using fast::executors::ThreadPool;

#include <chrono>

using namespace std::chrono_literals;
using namespace fast;

void AssertOn(ThreadPool& pool) {
  ASSERT_EQ(ThreadPool::current(), &pool);
}

TEST(TestFiber, JustWorks) {
    ThreadPool pool{3};
    pool.start();

    bool done = false;

    fibers::go(pool, [&]() {
      AssertOn(pool);
      done = true;
    });

    pool.wait();

    ASSERT_TRUE(done);

    pool.stop();
}

TEST(TestFiber, Child) {
    ThreadPool pool{3};
    pool.start();

    std::atomic<size_t> done{0};

    auto init = [&]() {
      AssertOn(pool);

      fibers::go([&]() {
        AssertOn(pool);
        ++done;
      });

      ++done;
    };

    fibers::go(pool, init);

    pool.wait();

    ASSERT_EQ(done.load(), 2);

    pool.stop();
}

TEST(TestFiber, RunInParallel) {
    ThreadPool pool{3};
    pool.start();

    std::atomic<size_t> completed{0};

    auto sleeper = [&]() {
      std::this_thread::sleep_for(3s);
      completed.fetch_add(1);
    };

    fibers::go(pool, sleeper);
    fibers::go(pool, sleeper);
    fibers::go(pool, sleeper);

    pool.wait();

    ASSERT_EQ(completed.load(), 3);
    pool.stop();
}

TEST(TestFiber, Yield1) {
    ThreadPool pool{1};
    pool.start();

    bool done = false;

    fibers::go(pool, [&] {
      fibers::yield();

      AssertOn(pool);
      done = true;
    });

    pool.wait();
    ASSERT_TRUE(done);

    pool.stop();
}

TEST(TestFiber, Yield2) {
    ThreadPool pool{1};

    enum State : int {
      Ping = 0,
      Pong = 1
    };

    int state = Ping;

    fibers::go(pool, [&] {
      for (size_t i = 0; i < 2; ++i) {
        ASSERT_EQ(state, Ping);
        state = Pong;

        fibers::yield();
      }
    });

    fibers::go(pool, [&] {
      for (size_t j = 0; j < 2; ++j) {
        ASSERT_EQ(state, Pong);
        state = Ping;

        fibers::yield();
      }
    });

    pool.start();

    pool.wait();
    pool.stop();
}

TEST(stopr, Yield3) {
    ThreadPool pool{4};

    static const size_t kYields = 1024;

    auto runner = [] {
      for (size_t i = 0; i < kYields; ++i) {
        fibers::yield();
      }
    };

    fibers::go(pool, runner);
    fibers::go(pool, runner);

    pool.start();

    pool.wait();
    pool.stop();
}

TEST(stopr, TwoPools1) {
    ThreadPool pool_1{4};
    ThreadPool pool_2{4};

    pool_1.start();
    pool_2.start();

    auto make_tester = [](ThreadPool& pool) {
      return [&pool]() {
        AssertOn(pool);
      };
    };

    fibers::go(pool_1, make_tester(pool_1));
    fibers::go(pool_2, make_tester(pool_2));

    pool_1.wait();
    pool_2.wait();

    pool_1.stop();
    pool_2.stop();
}

TEST(TestFiber, TwoPools2) {
    ThreadPool pool_1{4};
    pool_1.start();

    ThreadPool pool_2{4};
    pool_2.start();

    auto make_tester = [](ThreadPool& pool) {
      return [&pool]() {
        static const size_t kIterations = 128;

        for (size_t i = 0; i < kIterations; ++i) {
          AssertOn(pool);

          fibers::yield();

          fibers::go(pool, [&pool]() {
            AssertOn(pool);
          });
        }
      };
    };

    fibers::go(pool_1, make_tester(pool_1));
    fibers::go(pool_2, make_tester(pool_2));

    pool_1.wait();
    pool_2.wait();

    pool_1.stop();
    pool_2.stop();
}

class RacyCounter {
   public:
    void Increment() {
      value_.store(value_.load(std::memory_order_relaxed) + 1,
                   std::memory_order_relaxed);
    }
    size_t Get() const {
      return value_.load(std::memory_order_relaxed);
    }

   private:
    std::atomic<size_t> value_{0};
};

TEST(TestFiber, RacyCounter) {
    static const size_t kThreads = 4;
    static const size_t kFibers = 100;

    RacyCounter racy_counter;
    std::atomic<size_t> counter{0};

    executors::ThreadPool scheduler{kThreads};
    scheduler.start();

    for (size_t i = 0; i < kFibers; ++i) {
      fibers::go(scheduler, [&] {
        for (size_t i = 0; i < 1e4; ++i) {
          racy_counter.Increment();
          ++counter;

          if (i % 7 == 0) {
            fibers::yield();
          }
        }
      });
    };

    scheduler.wait();

    std::cout << "Threads: " << kThreads << std::endl
              << "Fibers: " << kFibers << std::endl
              << "Increments: " << counter.load() << std::endl
              << "Racy counter value: " << racy_counter.Get() << std::endl;

    ASSERT_LT(racy_counter.Get(), counter.load());

    scheduler.stop();
}
