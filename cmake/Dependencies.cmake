include(FetchContent)

set(BENCHMARK_ENABLE_TESTING off)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
)

FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.7.1
)

FetchContent_Declare(
        function2
        GIT_REPOSITORY https://github.com/Naios/function2.git
        GIT_TAG 4.2.2
)

FetchContent_Declare(
        expected
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG cmake_love
)

FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-22-1
)

# FetchContent_Declare(
#     boost
#     GIT_REPOSITORY "https://github.com/boostorg/boost.git"
#     GIT_TAG master
#     GIT_SUBMODULES "tools/cmake;libs/lockfree;libs/align;libs/assert;libs/array;libs/atomic;libs/config;libs/core;libs/static_assert;libs/integer;libs/throw_exception;libs/type_traits;libs/predef;libs/iterator;libs/preprocessor;libs/concept_check;libs/mpl;libs/utility;libs/parameter;libs/conversion;libs/io;libs/function;libs/tuple;libs/detail;libs/bind;libs/typeof;libs/smart_ptr;libs/function_types;libs/fusion"
#     GIT_PROGRESS TRUE
#     CONFIGURE_COMMAND ""  # tell CMake it's not a cmake project
# )

FetchContent_MakeAvailable(googletest googlebenchmark function2 expected asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)

set(GTEST_LIB gtest GTest::gtest_main)
set(BENCHMARK_LIB benchmark)
