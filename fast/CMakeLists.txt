set(LIBRARY_NAME "fastlib")
set(LIBRARY_SOURCES "")

macro (SRCS)
    foreach(ARG IN ITEMS ${ARGN})
        list(APPEND LIBRARY_SOURCES "./src/${ARG}")
    endforeach()
endmacro()


SRCS(
    sync/spinlock.cc
    sync/mutex.cc
    sync/futex.cc
)

message("Library sources : ${LIBRARY_SOURCES}")

add_library(fastlib STATIC "${LIBRARY_SOURCES}")
target_include_directories(fastlib PUBLIC "include")

add_subdirectory(tests)
add_subdirectory(bench)
