cmake_minimum_required(VERSION 3.0.0)

project(Foxtrot)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE -O3)
endif ()

include(config.cmake OPTIONAL)

file(GLOB_RECURSE SRC src/*.cpp src/*.c)

add_executable(foxtrot ${SRC})

target_include_directories(foxtrot PRIVATE src/)
target_link_libraries(foxtrot xengine imgui)

file(COPY assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

