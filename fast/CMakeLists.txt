set(LIBRARY_NAME "fastlib")
set(LIBRARY_SOURCES "")

macro (SRCS)
    foreach(ARG IN ITEMS ${ARGN})
        list(APPEND LIBRARY_SOURCES "./src/${ARG}")
    endforeach()
endmacro()

enable_language(ASM)

SRCS(
    sync/spinlock.cc
    sync/mutex.cc
    sync/futex.cc
    sync/condvar.cc
    sync/wait_group.cc

    executors/thread_pool.cc
    executors/manual.cc
    executors/strand.cc

    coro/ctx/context.S
    coro/ctx/execution.cc
    coro/ctx/machine.cc
    coro/coroutine.cc

    fibers/fiber.cc
    fibers/handle.cc
    fibers/sched.cc
    fibers/futex.cc
    fibers/mutex.cc
    fibers/wait_group.cc
)

message("Library sources : ${LIBRARY_SOURCES}")

add_library(fastlib STATIC "${LIBRARY_SOURCES}")
target_include_directories(fastlib PUBLIC "include")

add_subdirectory(tests)
add_subdirectory(bench)
