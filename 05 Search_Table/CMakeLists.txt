cmake_minimum_required(VERSION 3.17)
project(05_Search_Table C)

set(CMAKE_C_STANDARD 99)

add_executable(05_Search_Table main.c Search_Table.c print_table_console.c)