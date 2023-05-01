set(LIBRARY_NAME "fastlib")
set(LIBRARY_SOURCES "")

macro (SRCS)
    list(APPEND LIBRARY_SOURCES "./src/${ARGV}")
endmacro()


SRCS(
    sync/spinlock.cc
)

message("Library sources : ${LIBRARY_SOURCES}")

add_library(fastlib STATIC "${LIBRARY_SOURCES}")
target_include_directories(fastlib PUBLIC "include")

add_subdirectory(tests)
add_subdirectory(bench)
