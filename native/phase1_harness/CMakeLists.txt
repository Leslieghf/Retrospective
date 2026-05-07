cmake_minimum_required(VERSION 3.20)
project(retrospective_phase1_harness LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(phase1_harness
  ../src/retrospective_loader.cpp
  ../src/phase1_harness_main.cpp
)

target_include_directories(phase1_harness PRIVATE ../include)
target_link_libraries(phase1_harness PRIVATE ${CMAKE_DL_LIBS})

if(MSVC)
  target_compile_options(phase1_harness PRIVATE /W4 /permissive-)
else()
  target_compile_options(phase1_harness PRIVATE -Wall -Wextra -Wpedantic)
endif()
