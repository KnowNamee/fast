#include <gtest/gtest.h>

#include <sync/condvar.h>

#include <mutex>
#include <thread>

using namespace fast::sync;

class E {
public:
void await() {
    std::unique_lock lock(mutex_);
    while (!set_) {
    set_cond_.wait(lock);
    }
}

void set() {
    std::lock_guard guard(mutex_);
    set_ = true;
    set_cond_.notify_one();
}

void reset() {
    std::lock_guard guard(mutex_);
    set_ = false;
}

private:
bool set_{false};
std::mutex mutex_;
Condvar set_cond_;
};

TEST(CondvarTest, NotifyOne) {
    E pass;

    for (size_t i = 0; i < 3; ++i) {
      pass.reset();

      bool passed = false;

      std::thread waiter([&]() {
          {
            pass.await();
          }
          passed = true;
      });

      std::this_thread::sleep_for(std::chrono::seconds(1));

      ASSERT_FALSE(passed);

      pass.set();
      waiter.join();

      ASSERT_TRUE(passed);
    }
}

class L {
public:
void await() {
    std::unique_lock lock(mutex_);
    while (!released_) {
    released_cond_.wait(lock);
    }
}

void release() {
    std::lock_guard guard(mutex_);
    released_ = true;
    released_cond_.notify_all();
}

void reset() {
    std::lock_guard guard(mutex_);
    released_ = false;
}

private:
bool released_{false};
std::mutex mutex_;
Condvar released_cond_;
};

TEST(CondvarTest, NotifyAll) {
    L latch;

    for (size_t i = 0; i < 3; ++i) {
      latch.reset();

      std::atomic<size_t> passed{0};

      auto wait_routine = [&]() {
        latch.await();
        ++passed;
      };

      std::thread t1(wait_routine);
      std::thread t2(wait_routine);

      std::this_thread::sleep_for(std::chrono::seconds(1));

      ASSERT_EQ(passed.load(), 0);

      latch.release();

      t1.join();
      t2.join();

      ASSERT_EQ(passed.load(), 2);
    }
}

TEST(CondvarTest, NotifyManyTimes) {
    static const size_t kIterations = 1000'000;

    Condvar cv;
    for (size_t i = 0; i < kIterations; ++i) {
      cv.notify_one();
    }
}