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
      ${CMAKE_SOURCE_DIR}/src/dummyrenderer.cpp
      ${CMAKE_SOURCE_DIR}/include/dummyrenderer.hpp
      ${CMAKE_SOURCE_DIR}/src/vulkanrenderer.cpp
      ${CMAKE_SOURCE_DIR}/include/vulkanrenderer.hpp
      ${CMAKE_SOURCE_DIR}/include/shader.hpp
      ${CMAKE_SOURCE_DIR}/src/vkshader.cpp
      ${CMAKE_SOURCE_DIR}/include/vkshader.hpp
      ${CMAKE_SOURCE_DIR}/include/logger.hpp
      ${CMAKE_SOURCE_DIR}/src/dummylogger.cpp
      ${CMAKE_SOURCE_DIR}/include/dummylogger.hpp
      ${CMAKE_SOURCE_DIR}/src/debuglogger.cpp
      ${CMAKE_SOURCE_DIR}/include/debuglogger.hpp
      ${CMAKE_SOURCE_DIR}/src/servicelocator.cpp
      ${CMAKE_SOURCE_DIR}/include/servicelocator.hpp
      ${CMAKE_SOURCE_DIR}/src/eventmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/eventmanager.hpp
      ${CMAKE_SOURCE_DIR}/src/camera3d.cpp
      ${CMAKE_SOURCE_DIR}/include/camera3d.hpp
      ${CMAKE_SOURCE_DIR}/include/material.hpp
      ${CMAKE_SOURCE_DIR}/src/vkmaterial.cpp
      ${CMAKE_SOURCE_DIR}/include/vkmaterial.hpp
      ${CMAKE_SOURCE_DIR}/src/materialmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/materialmanager.hpp
      ${CMAKE_SOURCE_DIR}/src/dummymaterialmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/dummymaterialmanager.hpp
      ${CMAKE_SOURCE_DIR}/include/scriptengine.hpp
      ${CMAKE_SOURCE_DIR}/src/luascriptengine.cpp
      ${CMAKE_SOURCE_DIR}/include/luascriptengine.hpp
  )

  target_compile_definitions(CleanEngineVk PRIVATE GLM_FORCE_RADIANS)
  target_compile_definitions(CleanEngineVk PRIVATE RENDERER_VULKAN)

  target_include_directories(CleanEngineVk PUBLIC
    ${CMAKE_SOURCE_DIR}/include
  )

  if(MSVC)
    add_compile_options("/std:c++20")
    add_compile_definitions(NOMINMAX)
    target_link_directories(CleanEngineVk PUBLIC win64)
    target_link_libraries(CleanEngineVk glfw3.lib vulkan-1.lib)
  elseif(UNIX)
    target_link_libraries(CleanEngineVk -lglfw -lvulkan -lX11-xcb -ldl -lpthread -lassimp -lIL -lILU -llua5.3)
  endif()
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
      ${CMAKE_SOURCE_DIR}/src/dummyrenderer.cpp
      ${CMAKE_SOURCE_DIR}/include/dummyrenderer.hpp
      ${CMAKE_SOURCE_DIR}/src/openglrenderer.cpp
      ${CMAKE_SOURCE_DIR}/include/openglrenderer.hpp
      ${CMAKE_SOURCE_DIR}/include/shader.hpp
      ${CMAKE_SOURCE_DIR}/src/glshader.cpp
      ${CMAKE_SOURCE_DIR}/include/glshader.hpp
      ${CMAKE_SOURCE_DIR}/include/logger.hpp
      ${CMAKE_SOURCE_DIR}/src/dummylogger.cpp
      ${CMAKE_SOURCE_DIR}/include/dummylogger.hpp
      ${CMAKE_SOURCE_DIR}/src/debuglogger.cpp
      ${CMAKE_SOURCE_DIR}/include/debuglogger.hpp
      ${CMAKE_SOURCE_DIR}/src/servicelocator.cpp
      ${CMAKE_SOURCE_DIR}/include/servicelocator.hpp
      ${CMAKE_SOURCE_DIR}/src/eventmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/eventmanager.hpp
      ${CMAKE_SOURCE_DIR}/src/camera3d.cpp
      ${CMAKE_SOURCE_DIR}/include/camera3d.hpp
      ${CMAKE_SOURCE_DIR}/include/material.hpp
      ${CMAKE_SOURCE_DIR}/src/glmaterial.cpp
      ${CMAKE_SOURCE_DIR}/include/glmaterial.hpp
      ${CMAKE_SOURCE_DIR}/src/materialmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/materialmanager.hpp
      ${CMAKE_SOURCE_DIR}/src/dummymaterialmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/dummymaterialmanager.hpp
      ${CMAKE_SOURCE_DIR}/include/scriptengine.hpp
      ${CMAKE_SOURCE_DIR}/src/luascriptengine.cpp
      ${CMAKE_SOURCE_DIR}/include/luascriptengine.hpp
  )

  target_compile_definitions(CleanEngineGL PRIVATE GLM_FORCE_RADIANS)
  target_compile_definitions(CleanEngineGL PRIVATE RENDERER_OPENGL)

  target_include_directories(CleanEngineGL PUBLIC
    ${CMAKE_SOURCE_DIR}/include
  )

  if(MSVC)
    add_compile_options("/std:c++20")
    add_compile_definitions(NOMINMAX)
    target_link_directories(CleanEngineGL PUBLIC win64)
    target_link_libraries(CleanEngineGL glfw3.lib glew32.lib opengl32.lib)
  elseif(UNIX)
    target_link_libraries(CleanEngineGL -lglfw -lGLEW -lGL -ldl -lpthread -lassimp -lIL -lILU -llua5.3)
  endif()
endif()
