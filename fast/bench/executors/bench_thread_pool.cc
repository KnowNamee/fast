#include <benchmark/benchmark.h>

#include <executors/thread_pool.h>
#include <asio/thread_pool.hpp>
#include <asio/post.hpp>
#include <fibers/sched.h>

#include <iostream>

static void benchmarkAsio(benchmark::State& state) {
    for (auto _ : state) {
        asio::thread_pool pool(10);
        std::atomic<int> x = 0;
        for (size_t i = 0; i < state.range(0); ++i) {
            asio::post(pool, [&x]{
                x++;
            });
        }
        pool.wait();
        pool.stop();
    }
}

static void benchmarkLibrary(benchmark::State& state) {
    for (auto _ : state) {
        fast::executors::ThreadPool pool(10);
        pool.start();
        std::atomic<int> x = 0;
        for (size_t i = 0; i < state.range(0); ++i) {
            pool.submit([&x]{
                x++;
            });
        }
        pool.wait();
        pool.stop();
    }
}

static void benchmarkFibers(benchmark::State& state) {
    for (auto _ : state) {
        fast::executors::ThreadPool pool(10);
        pool.start();
        std::atomic<int> x = 0;
        for (size_t i = 0; i < state.range(0); ++i) {
            fast::fibers::go(pool,[&x] {
                x++;
            });
        }
        pool.wait();
        pool.stop();
    }
}

BENCHMARK(benchmarkAsio)
    ->Arg(100)->Arg(300)->Arg(500)->Arg(800)->Arg(1000)->Arg(1500)->Arg(2000)->Arg(6000)->Arg(10000)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkLibrary)
    ->Arg(100)->Arg(300)->Arg(500)->Arg(800)->Arg(1000)->Arg(1500)->Arg(2000)->Arg(6000)->Arg(10000)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkFibers)
    ->Arg(100)->Arg(300)->Arg(500)->Arg(800)->Arg(1000)->Arg(1500)->Arg(2000)->Arg(6000)->Arg(10000)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
