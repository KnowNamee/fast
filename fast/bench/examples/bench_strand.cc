#include <benchmark/benchmark.h>

#include <cstdlib>
#include <executors/thread_pool.h>
#include <executors/strand.h>
#include <fibers/sched.h>

#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

static void benchmarkStrand(benchmark::State& state) {
	for (auto _ : state) {
		fast::executors::ThreadPool pool(state.range(0));
		pool.start();

		fast::executors::Strand strand(pool);

		std::queue<int> q;
		for (size_t i = 0; i < 1000; ++i) {
			strand.submit([&]{
				q.push(i);
			});
		}
		pool.wait();

		for (size_t i = 0; i < 1000; ++i) {
			strand.submit([&]{
				q.pop();
			});
		}

		pool.wait();
		pool.stop();
	}
}

static void benchmarkMutex(benchmark::State& state) {
	for (auto _ : state) {
		fast::executors::ThreadPool pool(state.range(0));
		pool.start();

		std::mutex mu;
		std::queue<int> q;

		for (size_t i = 0; i < 1000; ++i) {
			pool.submit([&]{
				std::lock_guard<std::mutex> lock(mu);
				q.push(i);
			});
		}
		pool.wait();

		for (size_t i = 0; i < 1000; ++i) {
			pool.submit([&]{
				std::lock_guard<std::mutex> lock(mu);
				q.pop();
			});
		}

		pool.wait();
		pool.stop();
	}
}

BENCHMARK(benchmarkStrand)
	->Arg(1)->Arg(2)->Arg(4)->Arg(6)->Arg(10)->Arg(20)
    ->Unit(benchmark::kMillisecond);

BENCHMARK(benchmarkMutex)
	->Arg(1)->Arg(2)->Arg(4)->Arg(6)->Arg(10)->Arg(20)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
