cmake_minimum_required(VERSION 3.24)
project(challenge)

set(CMAKE_CXX_STANDARD 11)

find_package(MPI REQUIRED)
SET(CMAKE_CXX_COMPILER mpicxx)
SET(CMAKE_C_COMPILER mpicc)

add_executable(challenge main.cpp grep.cpp)

target_link_libraries(challenge PUBLIC MPI::MPI_CXX)