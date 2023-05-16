set(LIBS_LIST ${GTEST_LIB} ${LIBRARY_NAME})

function(add_gtest TEST_NAME)
    set(BINARY_SOURCES ${ARGN})

    add_executable(${TEST_NAME} ${BINARY_SOURCES})
    add_dependencies(${TEST_NAME} ${LIBS_LIST})
    target_link_libraries(${TEST_NAME} pthread ${LIBS_LIST})

    add_test(NAME ${TEST_NAME} COMMAND ${LIBS_LIST})
endfunction()

enable_testing()

add_gtest(test_sync_spinlock sync/test_spinlock.cc)

add_gtest(test_coro_coro coro/test_coro.cc)
