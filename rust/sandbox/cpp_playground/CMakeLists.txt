cmake_minimum_required(VERSION 3.20)
project(retrospective_cpp_playground LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cpp_playground src/main.cpp)

if(MSVC)
  target_compile_options(cpp_playground PRIVATE /W4 /permissive-)
else()
  target_compile_options(cpp_playground PRIVATE -Wall -Wextra -Wpedantic)
endif()
