cmake_minimum_required(VERSION 3.5)

project(event_frame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(event_frame main.cpp)

install(TARGETS event_frame
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
