cmake_minimum_required(VERSION 3.20)
project(JackCompiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_BUILD_TYPE Debug)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
FILE(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(INC ${PROJECT_SOURCE_DIR}/include)

include_directories(${INC})

add_executable(Compiler ${SRC})