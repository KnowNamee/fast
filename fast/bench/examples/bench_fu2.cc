#include <benchmark/benchmark.h>

#include <function2/function2.hpp>

template <typename Callable>
void bind(Callable callable) {
    std::move(callable)();
}

static void benchFunction2(benchmark::State& state) {
    using Fn = fu2::unique_function<void()>;
    static int counter = 0;

    for (auto _ : state) {
        benchmark::DoNotOptimize(counter);
        Fn f = [&]{
            counter++;
        };
        bind(std::move(f));
    }
}

static void benchFunction(benchmark::State& state) {
    using Fn = std::function<void()>;
    static int counter = 0;

    for (auto _ : state) {
        benchmark::DoNotOptimize(counter);
        Fn f = [&]{
            counter++;
        };
        bind(std::move(f));
    }
}

BENCHMARK(benchFunction)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(benchFunction2)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();
