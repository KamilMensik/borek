cmake_minimum_required(VERSION 3.16)
project(BOREK_ENGINE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "-g -Wall -O0 -fPIC")
set(CMAKE_CXX_FLAGS "-g -Wall -O0 -fPIC")

add_subdirectory(Borek)
add_subdirectory(Sandbox)


