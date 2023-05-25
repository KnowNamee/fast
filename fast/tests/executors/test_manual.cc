#include <gtest/gtest.h>

#include <executors/manual.h>
#include <executors/exe.h>

using fast::executors::ManualExecutor;
using fast::executors::IExecutor;

TEST(TestManual, JustWorks) {
    ManualExecutor manual;

    size_t step = 0;

    ASSERT_FALSE(manual.nonEmpty());

    ASSERT_FALSE(manual.runNext());
    ASSERT_EQ(manual.runAtMost(99), 0);

    manual.submit([&] {
      step = 1;
    });

    ASSERT_TRUE(manual.nonEmpty());
    ASSERT_EQ(manual.taskCount(), 1);

    ASSERT_EQ(step, 0);

    manual.submit([&] {
      step = 2;
    });

    ASSERT_EQ(manual.taskCount(), 2);

    ASSERT_EQ(step, 0);

    ASSERT_TRUE(manual.runNext());

    ASSERT_EQ(step, 1);

    ASSERT_TRUE(manual.nonEmpty());
    ASSERT_EQ(manual.taskCount(), 1);

    manual.submit([&] {
      step = 3;
    });

    ASSERT_EQ(manual.taskCount(), 2);

    ASSERT_EQ(manual.runAtMost(99), 2);
    ASSERT_EQ(step, 3);

    ASSERT_FALSE(manual.nonEmpty());
    ASSERT_FALSE(manual.runNext());
}

  class Looper {
   public:
    explicit Looper(IExecutor& e, size_t iters)
        : executor_(e),
          iters_left_(iters) {
    }

    void Start() {
      Submit();
    }

    void Iter() {
      --iters_left_;
      if (iters_left_ > 0) {
        Submit();
      }
    }

   private:
    void Submit() {
      executor_.submit([this] {
        Iter();
      });
    }

   private:
    IExecutor& executor_;
    size_t iters_left_;
  };

TEST(TestManual, RunAtMost) {
    ManualExecutor manual;

    Looper looper{manual, 256};
    looper.Start();

    size_t tasks = 0;
    do {
      tasks += manual.runAtMost(7);
    } while (manual.nonEmpty());

    ASSERT_EQ(tasks, 256);
  }

TEST(TestManual, Drain) {
    ManualExecutor manual;

    Looper looper{manual, 117};
    looper.Start();

    ASSERT_EQ(manual.drain(), 117);
  }