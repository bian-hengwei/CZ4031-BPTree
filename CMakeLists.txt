cmake_minimum_required(VERSION 3.24)
project(CZ4031Project1)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(CZ4031Project1
        src/main.cpp
        src/dataTypes.h
        src/storage.cpp
        src/storage.h src/address.cpp src/address.h)
