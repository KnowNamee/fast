set(LIBS_LIST ${GTEST_LIB} ${LIBRARY_NAME})

function(add_gtest TEST_NAME)
    set(BINARY_SOURCES ${ARGN})

    add_executable(${TEST_NAME} ${BINARY_SOURCES})
    add_dependencies(${TEST_NAME} ${LIBS_LIST})
    target_link_libraries(${TEST_NAME} pthread ${LIBS_LIST})

    add_test(NAME ${TEST_NAME} COMMAND ${LIBS_LIST})
endfunction()

enable_testing()

add_gtest(test_sync_spinlock            sync/test_spinlock.cc)
add_gtest(test_sync_mutex               sync/test_mutex.cc)
add_gtest(test_sync_condvar             sync/test_condvar.cc)
add_gtest(test_sync_wait_group          sync/test_wait_group.cc)

add_gtest(test_executors_manual         executors/test_manual.cc)
add_gtest(test_executors_strand         executors/test_strand.cc)
add_gtest(test_executors_thread_pool    executors/test_thread_pool.cc)

add_gtest(test_fibers_fibers            fibers/test_fibers.cc)
add_gtest(test_fibers_mutex             fibers/test_mutex.cc)
add_gtest(test_fibers_wait_group        fibers/test_wait_group.cc)

add_gtest(test_coro_coro                coro/test_coro.cc)
