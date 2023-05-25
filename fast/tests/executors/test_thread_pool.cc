#include <gtest/gtest.h>

#include <executors/thread_pool.h>

#include <atomic>
#include <chrono>
#include <thread>

using fast::executors::ThreadPool;

using namespace std::chrono_literals;

template <typename E, typename T>
void submit(E& e, T&& t) {
    e.submit(std::move(t));
}

TEST(TestThreadPool, JustWorks) {
    ThreadPool pool{4};

    pool.start();

    submit(pool, [] {
      std::cout << "Hello from thread pool!" << std::endl;
    });

    pool.wait();
    pool.stop();
}

TEST(TestThreadPool, Wait) {
    ThreadPool pool{4};

    pool.start();

    bool done = false;

    submit(pool, [&] {
      std::this_thread::sleep_for(1s);
      done = true;
    });

    pool.wait();
    pool.stop();

    ASSERT_TRUE(done);
}

TEST(TestThreadPool, MultiWait) {
    ThreadPool pool{1};

    pool.start();

    for (size_t i = 0; i < 3; ++i) {
      bool done = false;

      submit(pool, [&] {
        std::this_thread::sleep_for(1s);
        done = true;
      });

      pool.wait();

      ASSERT_TRUE(done);
    }

    pool.stop();
}

TEST(TestThreadPool, ManyTasks) {
    ThreadPool pool{4};

    pool.start();

    static const size_t kTasks = 17;

    std::atomic<size_t> tasks{0};

    for (size_t i = 0; i < kTasks; ++i) {
      submit(pool, [&] {
        ++tasks;
      });
    }

    pool.wait();
    pool.stop();

    ASSERT_EQ(tasks.load(), kTasks);
}

TEST(TestThreadPool, Parallel) {
    ThreadPool pool{4};

    pool.start();

    std::atomic<size_t> tasks{0};

    submit(pool, [&] {
      std::this_thread::sleep_for(1s);
      ++tasks;
    });

    submit(pool, [&] {
      ++tasks;
    });

    std::this_thread::sleep_for(100ms);

    ASSERT_EQ(tasks.load(), 1);

    pool.wait();
    pool.stop();

    ASSERT_EQ(tasks.load(), 2);
}

TEST(TestThreadPool, TwoPools) {
    ThreadPool pool1{1};
    ThreadPool pool2{1};

    pool1.start();
    pool2.start();

    std::atomic<size_t> tasks{0};

    submit(pool1, [&] {
      std::this_thread::sleep_for(1s);
      ++tasks;
    });

    submit(pool2, [&] {
      std::this_thread::sleep_for(1s);
      ++tasks;
    });

    pool2.wait();
    pool2.stop();

    pool1.wait();
    pool1.stop();

    ASSERT_EQ(tasks.load(), 2);
}

TEST(TestThreadPool, stop) {
    ThreadPool pool{1};

    pool.start();

    for (size_t i = 0; i < 3; ++i) {
      submit(pool, [] {
        std::this_thread::sleep_for(128ms);
      });
    }

    pool.stop();
}

TEST(TestThreadPool, current) {
    ThreadPool pool{1};

    pool.start();

    ASSERT_EQ(ThreadPool::current(), nullptr);

    submit(pool, [&] {
      ASSERT_EQ(ThreadPool::current(), &pool);
    });

    pool.wait();
    pool.stop();
}

TEST(TestThreadPool, SubmitAfterwait) {
    ThreadPool pool{4};

    pool.start();

    bool done = false;

    submit(pool, [&] {
      std::this_thread::sleep_for(500ms);

      submit(*ThreadPool::current(), [&] {
        std::this_thread::sleep_for(500ms);
        done = true;
      });
    });

    pool.wait();
    pool.stop();

    ASSERT_TRUE(done);
}

TEST(TestThreadPool, TaskLifetime) {
    ThreadPool pool{4};

    pool.start();

    std::atomic<int> dead{0};

    class Task {
     public:
      Task(std::atomic<int>& done) : counter_(done) {
      }

      ~Task() {
        if (done_) {
          counter_.fetch_add(1);
        }
      }

      void operator()() {
        std::this_thread::sleep_for(100ms);
        done_ = true;
      }

     private:
      bool done_{false};
      std::atomic<int>& counter_;
    };

    for (int i = 0; i < 4; ++i) {
      submit(pool, Task(dead));
    }
    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(dead.load(), 4);

    pool.wait();
    pool.stop();
}

TEST(TestThreadPool, Racy) {
    ThreadPool pool{4};

    pool.start();

    std::atomic<int> shared_counter{0};
    std::atomic<int> tasks{0};

    for (size_t i = 0; i < 100500; ++i) {
      submit(pool, [&] {
        int old = shared_counter.load();
        shared_counter.store(old + 1);

        ++tasks;
      });
    }

    pool.wait();
    pool.stop();

    std::cout << "Racy counter value: " << shared_counter << std::endl;

    ASSERT_EQ(tasks.load(), 100500);
    ASSERT_LE(shared_counter.load(), 100500);
}