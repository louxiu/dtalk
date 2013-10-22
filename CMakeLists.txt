cmake_minimum_required(VERSION 2.6)

project(dtalk)

add_definitions(-std=c99)

add_executable(dtalk-s src/dtalk-s/dtalk-s.c)
add_executable(dtalk-c src/dtalk-c/dtalk-c.c)
