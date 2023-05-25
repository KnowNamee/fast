#pragma once

namespace fast::fibers {

class Fiber;

class FiberHandle {
  friend class Fiber;

 public:
  FiberHandle();
  
  static FiberHandle Invalid() {
    return FiberHandle(nullptr);
  }

  bool IsValid() const;
  void Schedule();

 private:
  explicit FiberHandle(Fiber* fiber);

  Fiber* Release();

 private:
  Fiber* fiber_;
};

}