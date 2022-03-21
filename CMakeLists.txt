cmake_minimum_required(VERSION 3.10)

project(CleanEngine LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(version.h.in version.h)

add_executable(CleanEngine
  ${CMAKE_SOURCE_DIR}/main.cpp
  ${CMAKE_SOURCE_DIR}/src/enginecore.cpp
  ${CMAKE_SOURCE_DIR}/include/enginecore.hpp
  ${CMAKE_SOURCE_DIR}/src/renderer.cpp
  ${CMAKE_SOURCE_DIR}/include/renderer.hpp
)

target_include_directories(CleanEngine PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(CleanEngine -lglfw -lvulkan -ldl -lpthread)
target_include_directories(CleanEngine PUBLIC "${PROJECT_BINARY_DIR}")
