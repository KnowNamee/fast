#include <benchmark/benchmark.h>

#include <sync/spinlock.h>
#include <types.h>

#include <mutex>
#include <thread>
#include <pthread.h>

struct SpinLockBaseline {
    void lock() {
        while (_atomic.exchange(1)) {
            std::this_thread::yield();
        }
    }

    void unlock() {
        _atomic.exchange(0);
    }

 private:
    std::atomic<fast::u32> _atomic{0};
};

struct PThreadSpinLock {
    PThreadSpinLock() {
        pthread_spin_init(&_lock, _shared);
    }

    void lock() {
        pthread_spin_lock(&_lock);
    }

    void unlock() {
        pthread_spin_unlock(&_lock);
    }    

 private:
    pthread_spinlock_t _lock;
    int _shared;
};

template <typename BasicLockable>
struct Counter {
    void count() {
        for (size_t i = 0; i < 100000; ++i) {
            std::lock_guard<BasicLockable> guard(lockable);
            state++;
        }
        for (size_t i = 0; i < 100000; ++i) {
            std::lock_guard<BasicLockable> guard(lockable);
            state--;
        }
    }

    BasicLockable lockable;
    int state = 0;
};

static Counter<SpinLockBaseline> baselineSpinLockCounter;
static Counter<fast::sync::TASSpinLock> fastSpinLockCounter;
static Counter<PThreadSpinLock> pthreadSpinLockCounter;

static void benchmarkBaselineSpinLockCounter(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(baselineSpinLockCounter.state);
        baselineSpinLockCounter.count();
    }
    if (state.thread_index() == 0 && baselineSpinLockCounter.state != 0) {
        throw std::runtime_error("BM_FastMutex : Invalid final state of basline counter");
    }
}

static void benchmarkFastSpinLockCounter(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(fastSpinLockCounter.state);
        fastSpinLockCounter.count();
    }
    if (state.thread_index() == 0 && fastSpinLockCounter.state != 0) {
        throw std::runtime_error("BM_FastMutex : Invalid final state of fast spinlock counter");
    }
}

static void benchmarkPthreadSpinLockCounter(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(pthreadSpinLockCounter.state);
        pthreadSpinLockCounter.count();
    }
    if (state.thread_index() == 0 && pthreadSpinLockCounter.state != 0) {
        throw std::runtime_error("BM_FastMutex : Invalid final state of pthread spinlock counter");
    }
}

BENCHMARK(benchmarkBaselineSpinLockCounter)
    ->Threads(1)->Threads(2)->Threads(4)->Threads(6)->Threads(10)->Threads(20)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkFastSpinLockCounter)
    ->Threads(1)->Threads(2)->Threads(4)->Threads(6)->Threads(10)->Threads(20)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkPthreadSpinLockCounter)
    ->Threads(1)->Threads(2)->Threads(4)->Threads(6)->Threads(10)->Threads(20)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
