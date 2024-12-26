cmake_minimum_required(VERSION 3.10)
project(watcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_executable(watcher watcher.cpp)

target_link_libraries(watcher 
    OpenSSL::SSL
    OpenSSL::Crypto
)