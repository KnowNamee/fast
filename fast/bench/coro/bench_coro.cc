#include <benchmark/benchmark.h>

#include <coro/coroutine.h>

static void benchmarkCoroSwitchContext(benchmark::State& state) {
  fast::coro::Coroutine coro([&state](){
    while (true) {
      fast::coro::Coroutine::suspend();
    }
  });
  for (auto _ : state) {
    coro.resume();
	}
}

BENCHMARK(benchmarkCoroSwitchContext)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();
