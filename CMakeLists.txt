cmake_minimum_required(VERSION 2.6)

project(dtalk)

add_definitions(-std=c99 -g)

include_directories(src/lib src/dtalk-s-lib)
find_package(Threads)

add_executable(dtalk-s 
  src/dtalk-s/dtalk-s.c
  src/lib/debug.c
  src/dtalk-s-lib/daemon.c
  src/lib/library.c
  src/lib/utils.c
)
add_executable(dtalk-c 
  src/dtalk-c/dtalk-c.c
  src/lib/debug.c
)

target_link_libraries(dtalk-s ${CMAKE_THREAD_LIBS_INIT})