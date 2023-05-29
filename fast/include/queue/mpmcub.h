#pragma once

#include <optional>
#include <condition_variable>
#include <queue>

#include <types.h>
#include <sync/condvar.h>
#include <sync/mutex.h>

namespace fast::queue {

namespace {
    using Condvar = sync::Condvar;
    using Mutex = sync::Mutex;
}  // namespace anonymous

template <typename T>
class MPMCUnboundedBlockingQueue {
 public:
  bool Put(T value) {
    std::lock_guard<Mutex> lock(_mutex);
    if (_isClosed) {
      return false;
    }
    _buffer.emplace_back(std::move(value));
    _notEmpty.notify_one();
    return true;
  }

  std::optional<T> Take() {
    std::unique_lock<Mutex> lock(_mutex);
    _notEmpty.wait(lock, [&] {
      return !_buffer.empty() || _isClosed;
    });
    if (_isClosed) {
      return std::nullopt;
    }

    T value = std::move(_buffer.front());
    _buffer.pop_front();

    return value;
  }

  void Close() {
    std::lock_guard<Mutex> lock(_mutex);
    _isClosed = true;
    _notEmpty.notify_all();
  }

 private:
  bool _isClosed{false};
  Mutex _mutex;
  Condvar _notEmpty;

  std::deque<T> _buffer;
};

}