include(FetchContent)

set(BENCHMARK_ENABLE_TESTING off)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0)

FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.7.1)

FetchContent_MakeAvailable(googletest googlebenchmark)

set(GTEST_LIB gtest GTest::gtest_main)
set(BENCHMARK_LIB benchmark)
