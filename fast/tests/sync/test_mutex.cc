#include <chrono>
#include <gtest/gtest.h>

#include <sync/mutex.h>
#include <thread>

using namespace fast::sync;

TEST(TestMutex, LockUnlock) {
    Mutex mutex;
    mutex.lock();
    mutex.unlock();
  }

TEST(TestMutex, SequentialLockUnlock) {
    Mutex mutex;
    mutex.lock();
    mutex.unlock();
    mutex.lock();
    mutex.unlock();
  }

TEST(TestMutex, NoSharedLocations) {
    Mutex mutex;
    mutex.lock();

    Mutex mutex2;
    mutex2.lock();
    mutex2.unlock();

    mutex.unlock();
}

TEST(TestMutex, MutualExclusion) {
    Mutex mutex;
    bool cs = false;

    std::thread locker([&]() {
      mutex.lock();
      cs = true;
      std::this_thread::sleep_for(std::chrono::seconds(3));
      cs = false;
      mutex.unlock();
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    mutex.lock();
    ASSERT_FALSE(cs);
    mutex.unlock();

    locker.join();
}