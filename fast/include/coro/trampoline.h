#pragma once

namespace fast::coro {

struct ITrampoline {
  virtual ~ITrampoline() = default;
  virtual void run() noexcept = 0;
};

}
