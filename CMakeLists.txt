cmake_minimum_required(VERSION 3.25)
project(fablab C)

set(CMAKE_C_STANDARD 99)

add_executable(fablab main.c equipment.h equipment.c users.h users.c meetings.h meetings.c)
