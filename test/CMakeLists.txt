cmake_minimum_required(VERSION 3.0.0)
project(shell_test VERSION 1.0.0 LANGUAGES C)

include_directories(../include)
add_executable(shell_test main.c ../shell.c)
