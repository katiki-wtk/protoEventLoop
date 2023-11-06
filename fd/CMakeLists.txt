cmake_minimum_required(VERSION 3.10)
enable_language(CXX)

project(poc_eventloop)
set(VERSION 1.0.0)

add_subdirectory(lib)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

set(TARGETNAME poc_eventloop)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++17 -Wno-psabi")

include_directories(. ${PROJECT_SOURCE_DIR})

add_executable(${TARGETNAME} main.cpp)

target_link_libraries(${TARGETNAME} eventloop)

install(TARGETS ${LIBNAME} DESTINATION /usr/lib)
