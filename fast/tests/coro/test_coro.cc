#include <gtest/gtest.h>

#include <coro/coroutine.h>

#include <vector>
#include <thread>

using namespace fast::coro;

TEST(Coro, Simple) {
    std::vector<uint32_t> result;
    Coroutine c([&result](){
        result.push_back(2);
        Coroutine::suspend();
        result.push_back(4);
    });
    result.push_back(1);
    c.resume();
    result.push_back(3);
    c.resume();
    EXPECT_TRUE(c.completed());
    EXPECT_EQ(result.size(), 4ull);
    EXPECT_EQ(result[0], 1ull);
    EXPECT_EQ(result[1], 2ull);
    EXPECT_EQ(result[2], 3ull);
    EXPECT_EQ(result[3], 4ull);
}

TEST(CoroutineTest, SimpleTest) {
    Coroutine co([] {
        Coroutine::suspend();
    });

    ASSERT_FALSE(co.completed());
    co.resume();
    ASSERT_FALSE(co.completed());
    co.resume();
    ASSERT_TRUE(co.completed());
}

TEST(CoroutineTest, Interleaving) {
    int step = 0;

    Coroutine a([&] {
      ASSERT_EQ(step, 0);
      step = 1;
      Coroutine::suspend();
      ASSERT_EQ(step, 2);
      step = 3;
    });

    Coroutine b([&] {
      ASSERT_EQ(step, 1);
      step = 2;
      Coroutine::suspend();
      ASSERT_EQ(step, 3);
      step = 4;
    });

    a.resume();
    b.resume();

    ASSERT_EQ(step, 2);

    a.resume();
    b.resume();

    ASSERT_TRUE(a.completed());
    ASSERT_TRUE(b.completed());

    ASSERT_EQ(step, 4);
}

struct Threads {
    template <typename F>
    void Run(F task) {
        std::thread t([task = std::move(task)]() mutable {
        task();
        });
        t.join();
    }
};

TEST(CoroutineTest, Threads) {
    size_t steps = 0;

    Coroutine co([&steps] {
      ++steps;
      Coroutine::suspend();
      ++steps;
      Coroutine::suspend();
      ++steps;
    });

    auto resume = [&co] {
      co.resume();
    };

    // Simulate fiber running in thread pool
    Threads threads;
    threads.Run(resume);
    threads.Run(resume);
    threads.Run(resume);

    ASSERT_EQ(steps, 3);
}

TEST(CoroutineTest, Pipeline) {
    const size_t kSteps = 1;

    size_t step_count = 0;

    Coroutine a([&] {
      Coroutine b([&] {
        for (size_t i = 0; i < kSteps; ++i) {
          ++step_count;
          Coroutine::suspend();
        }
      });

      while (!b.completed()) {
        b.resume();
        Coroutine::suspend();
      }
    });

    while (!a.completed()) {
      a.resume();
    }

    ASSERT_EQ(step_count, kSteps);
}

TEST(CoroutineTest, MemoryLeak) {
    auto shared_ptr = std::make_shared<int>(42);
    std::weak_ptr<int> weak_ptr = shared_ptr;

    {
      auto routine = [ptr = std::move(shared_ptr)] {};
      Coroutine co(routine);
      co.resume();
    }

    ASSERT_FALSE(weak_ptr.lock());
}
