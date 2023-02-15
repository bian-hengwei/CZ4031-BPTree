cmake_minimum_required(VERSION 3.10.0)

project(CZ4031Project1 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

include_directories(inc)

add_executable(CZ4031Project1
        src/main.cpp
        src/storage.cpp
        )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
