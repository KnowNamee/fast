#include <benchmark/benchmark.h>

#include <mutex>
#include <stdexcept>
#include <thread>

#include <sync/mutex.h>

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

static Counter<fast::sync::Mutex> fastMutexCounter;
static Counter<std::mutex> posixMutexCounter;

static void benchmarkPosixMutex(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(posixMutexCounter.state);
        posixMutexCounter.count();
    }
}

static void benchmarkFastMutex(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(fastMutexCounter.state);
        fastMutexCounter.count();
    }
    if (state.thread_index() == 0 && fastMutexCounter.state != 0) {
        throw std::runtime_error("BM_FastMutex : Invalid final state of counter");
    }
}

BENCHMARK(benchmarkPosixMutex)
    ->Threads(1)->Threads(2)->Threads(4)->Threads(6)->Threads(10)->Threads(20)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkFastMutex)
    ->Threads(1)->Threads(2)->Threads(4)->Threads(6)->Threads(10)->Threads(20)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
