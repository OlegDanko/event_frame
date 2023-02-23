cmake_minimum_required(VERSION 3.5)

project(event_frame LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/utils)

file(GLOB SRC
    src/event_frame/EventChannel.cpp
    src/event_frame/EventSpawner.cpp
)

add_library(event_frame STATIC ${SRC})
target_link_libraries(event_frame utils)
include_directories(event_frame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/event_frame)
include_directories(event_frame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/utils/include)

install(TARGETS event_frame
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
