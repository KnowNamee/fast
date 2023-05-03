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
    std::lock_guard<Mutex> lock(mu_);
    if (closed_) {
      return false;
    }
    buffer_.push(std::move(value));
    for_not_empty_.notify_one();
    return true;
  }

  std::optional<T> Take() {
    std::unique_lock<Mutex> lock(mu_);
    for_not_empty_.wait(lock, [&] {
      return !buffer_.empty() || (buffer_.empty() && closed_);
    });
    if (buffer_.empty() && closed_) {
      return std::nullopt;
    }

    T value = std::move(buffer_.front());
    buffer_.pop();

    return value;
  }

  void Close() {
    std::lock_guard<Mutex> lock(mu_);
    closed_ = true;
    for_not_empty_.notify_all();
  }

 private:
  bool closed_{false};
  Mutex mu_;
  Condvar for_not_empty_;

  std::queue<T> buffer_;
};

}