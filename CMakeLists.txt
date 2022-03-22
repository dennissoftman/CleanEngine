cmake_minimum_required(VERSION 3.10)

project(CleanEngine LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -Og -ggdb")

option(COMPILE_GL "Compile using OpenGL renderer" ON)
option(COMPILE_VK "Compile using Vulkan renderer" OFF)

# Vulkan version
if(COMPILE_VK)
  add_executable(CleanEngineVk
    ${CMAKE_SOURCE_DIR}/main.cpp
  )

  target_sources(CleanEngineVk
    PUBLIC
      ${CMAKE_SOURCE_DIR}/src/enginecore.cpp
      ${CMAKE_SOURCE_DIR}/include/enginecore.hpp
      ${CMAKE_SOURCE_DIR}/include/renderer.hpp
      ${CMAKE_SOURCE_DIR}/src/vulkanrenderer.cpp
      ${CMAKE_SOURCE_DIR}/include/vulkanrenderer.hpp
      ${CMAKE_SOURCE_DIR}/include/shader.hpp
      ${CMAKE_SOURCE_DIR}/src/vkshader.cpp
      ${CMAKE_SOURCE_DIR}/include/vkshader.hpp
  )

  target_compile_definitions(CleanEngineVk PRIVATE RENDERER_VULKAN)

  target_include_directories(CleanEngineVk PUBLIC
    ${CMAKE_SOURCE_DIR}/include
  )

  target_link_libraries(CleanEngineVk -lglfw -lvulkan -lX11-xcb -ldl -lpthread)
endif()
# OpenGL version

if(COMPILE_GL)
  add_executable(CleanEngineGL
    ${CMAKE_SOURCE_DIR}/main.cpp

  )

  target_sources(CleanEngineGL
    PUBLIC
      ${CMAKE_SOURCE_DIR}/src/enginecore.cpp
      ${CMAKE_SOURCE_DIR}/include/enginecore.hpp
      ${CMAKE_SOURCE_DIR}/include/renderer.hpp
      ${CMAKE_SOURCE_DIR}/src/openglrenderer.cpp
      ${CMAKE_SOURCE_DIR}/include/openglrenderer.hpp
      ${CMAKE_SOURCE_DIR}/include/shader.hpp
      ${CMAKE_SOURCE_DIR}/src/glshader.cpp
      ${CMAKE_SOURCE_DIR}/include/glshader.hpp
  )

  target_compile_definitions(CleanEngineGL PRIVATE RENDERER_OPENGL)

  target_include_directories(CleanEngineGL PUBLIC
    ${CMAKE_SOURCE_DIR}/include
  )

  target_link_libraries(CleanEngineGL -lglfw -lGLEW -lGL -ldl -lpthread)
endif()
