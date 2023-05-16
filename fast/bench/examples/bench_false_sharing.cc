#include <benchmark/benchmark.h>

#include <atomic>

class WithFalseSharing {
 public:
	void addToFirstCounter() {
		_counter1.fetch_add(1);
	}

	void addToSecondCounter() {
		_counter2.fetch_add(1);
	}

	std::pair<int, int> get() {
		return {_counter1.load(), _counter2.load()};
	}

	std::atomic<int> _counter1{0};
	std::atomic<int> _counter2{0};
};

class WithoutFalseSharing {
 public:
	void addToFirstCounter() {
		_counter1.fetch_add(1);
	}

	void addToSecondCounter() {
		_counter2.fetch_add(1);
	}

	std::pair<int, int> get() {
		return {_counter1.load(), _counter2.load()};
	}

	alignas(64) std::atomic<int> _counter1{0};
	alignas(64) std::atomic<int> _counter2{0};
};

static WithFalseSharing with;
static WithoutFalseSharing without;

static void benchmarkWithFalseSharing(benchmark::State& state) {
	int i = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(with._counter1);
		benchmark::DoNotOptimize(with._counter2);
		if (i++ % 2 == 0) {
		with.addToFirstCounter();
		} else {
			with.addToSecondCounter();
		}
		with.get();
	}
}

static void benchmarkWithoutFalseSharing(benchmark::State& state) {
	int i = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(without._counter1);
		benchmark::DoNotOptimize(without._counter2);
		if (i++ % 2 == 0) {
			without.addToFirstCounter();
		} else {
			without.addToSecondCounter();
		}
		with.get();
	}
}

BENCHMARK(benchmarkWithoutFalseSharing)
    ->Threads(1)->Threads(2)->Threads(4)->Threads(6)->Threads(10)->Threads(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(benchmarkWithFalseSharing)
    ->Threads(1)->Threads(2)->Threads(4)->Threads(6)->Threads(10)->Threads(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();
