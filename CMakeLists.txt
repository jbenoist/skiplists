set(DCMAKE_C_COMPILER "/usr/bin/gcc")
cmake_minimum_required (VERSION 2.8.6)
project (skiplist)

include_directories(.)
add_executable(test_linear test/linear.c skiplist.c quad.c)
add_executable(test_rigorous test/rigorous.c skiplist.c quad.c)
add_executable(test_simple test/simple.c skiplist.c quad.c)
add_definitions(-O3 -ggdb -Wall -D_GNU_SOURCE)

