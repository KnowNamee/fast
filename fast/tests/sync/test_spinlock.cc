#include <gtest/gtest.h>

#include <sync/spinlock.h>

using namespace fast::sync;

TEST(SpinlockTest, LockUnlock) {
    TASSpinLock spinlock;
    spinlock.lock();
    spinlock.unlock();
}

TEST(SpinlockTest, SequentialLockUnlock) {
    TASSpinLock spinlock;
    spinlock.lock();
    spinlock.unlock();

    spinlock.lock();
    spinlock.unlock();
}



