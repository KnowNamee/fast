set(LIBS_LIST ${BENCHMARK_LIB} ${LIBRARY_NAME})

function(add_benchmark TEST_NAME)
    set(BINARY_SOURCES ${ARGN})

    add_executable(${TEST_NAME} ${BINARY_SOURCES})
    add_dependencies(${TEST_NAME} ${LIBS_LIST})
    target_link_libraries(${TEST_NAME} pthread ${LIBS_LIST})

    add_test(NAME ${TEST_NAME} COMMAND ${LIBS_LIST})
endfunction()

enable_testing()

add_benchmark(bench_sync_spinlock           sync/bench_spinlock.cc)
add_benchmark(bench_sync_mutex              sync/bench_mutex.cc)

add_benchmark(bench_coro_coro               coro/bench_coro.cc)

add_benchmark(bench_examples_false_sharing  examples/bench_false_sharing.cc)
