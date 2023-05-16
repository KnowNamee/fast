#include <sync/spinlock.h>

#include <thread>

namespace fast::sync {

void TASSpinLock::lock() {
	while (!_atomic.exchange(1, std::memory_order_acquire)) {
		while (_atomic.load(std::memory_order_relaxed)) {
			std::this_thread::yield();
		}
	}
}

void TASSpinLock::unlock() {
	_atomic.store(0, std::memory_order_release);
}

}
