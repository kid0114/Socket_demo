cmake_minimum_required(VERSION 3.22)
project(Socket_demo)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(Socket_demo
        src/Client.cpp
        src/Client.h
        src/cmdparse.cpp
        src/cmdparse.h
        src/Server.cpp
        src/Server.h
        src/test_client.cpp
        src/test_server.cpp)
