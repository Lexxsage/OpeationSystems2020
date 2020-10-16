cmake_minimum_required(VERSION 3.17)
project(06_ClockTable C)

set(CMAKE_C_STANDARD 99)

add_executable(06_ClockTable main.c SearchTable.h SearchTable.c print_table_console.c)