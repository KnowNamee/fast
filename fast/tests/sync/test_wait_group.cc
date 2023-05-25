#include <gtest/gtest.h>

#include <sync/wait_group.h>
#include <thread>

using namespace fast::sync;

TEST(TestWaitGroup, JustWorks) {
    WaitGroup wg;

    wg.add(1);
    wg.done();
    wg.wait();
}

TEST(TestWaitGroup, InitZero) {
    WaitGroup wg;
    wg.wait();
}

TEST(TestWaitGroup, AddCount) {
    WaitGroup wg;

    wg.add(7);

    for (size_t i = 0; i < 7; ++i) {
      wg.done();
    }

    wg.wait();
}

TEST(TestWaitGroup, Wait) {
    WaitGroup wg;
    bool ready = false;

    wg.add(1);

    std::thread producer([&] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      ready = true;
      wg.done();
    });

    wg.wait();
    ASSERT_TRUE(ready);

    producer.join();
}

TEST(TestWaitGroup, MultiWait) {
    std::vector<std::thread> threads;

    WaitGroup wg;

    std::atomic<size_t> work{0};

    static const size_t kWorkers = 3;
    static const size_t kWaiters = 4;

    wg.add(kWorkers);

    for (size_t i = 0; i < kWaiters; ++i) {
      threads.emplace_back([&] {
        wg.wait();
        ASSERT_EQ(work.load(), kWorkers);
      });
    }

    for (size_t i = 1; i <= kWorkers; ++i) {
      threads.emplace_back([&, i] {
        std::this_thread::sleep_for(std::chrono::milliseconds(256) * i);
        ++work;
        wg.done();
      });
    }

    for (auto&& t : threads) {
      t.join();
    }
}
