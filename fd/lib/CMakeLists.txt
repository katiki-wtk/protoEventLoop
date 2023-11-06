cmake_minimum_required(VERSION 3.10)
enable_language(CXX)

project(eventloop)
set(VERSION 1.0.0)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++17 -Wno-psabi")

set(LIBNAME eventloop)

include_directories(. ${PROJECT_SOURCE_DIR})

FILE(GLOB_RECURSE sources *.cpp )
add_library(${LIBNAME} SHARED ${sources})
set_target_properties(${LIBNAME} PROPERTIES SOVERSION 1)

install(TARGETS ${LIBNAME} DESTINATION /usr/lib)
