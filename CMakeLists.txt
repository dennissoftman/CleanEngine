cmake_minimum_required(VERSION 3.10)

project(CleanEngine LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-mavx2")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -Og -ggdb")

option(COMPILE_GL "Compile using OpenGL renderer" ON)
option(COMPILE_VK "Compile using Vulkan renderer" OFF)

option(USE_BULLET "Compile using Bullet physics engien" ON)
option(BULLET_MULTITHREADED "Compile using multithreaded Bullet3" OFF)

option(USE_GLFW "Use GLFW library" ON)

option(USE_IMGUI "Use bloat-free imgui library" ON)
option(USE_CEGUI "Use Crazy Eddie's GUI library" OFF)

# Vulkan version
# TODO
if(COMPILE_VK)
  message(WARNING "Nothing for vulkan could be built")
endif()

# OpenGL version

if(COMPILE_GL)
  add_executable(CleanEngineGL
    ${CMAKE_SOURCE_DIR}/main.cpp
  )

  target_sources(CleanEngineGL
    PUBLIC
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
      ${CMAKE_SOURCE_DIR}/src/material.cpp
      ${CMAKE_SOURCE_DIR}/include/material.hpp
      ${CMAKE_SOURCE_DIR}/src/glmaterial.cpp
      ${CMAKE_SOURCE_DIR}/include/glmaterial.hpp
      ${CMAKE_SOURCE_DIR}/src/materialmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/materialmanager.hpp
      ${CMAKE_SOURCE_DIR}/src/modelmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/modelmanager.hpp
      ${CMAKE_SOURCE_DIR}/include/scriptengine.hpp
      ${CMAKE_SOURCE_DIR}/src/luascriptengine.cpp
      ${CMAKE_SOURCE_DIR}/include/luascriptengine.hpp
      ${CMAKE_SOURCE_DIR}/src/audiomanager.cpp
      ${CMAKE_SOURCE_DIR}/include/audiomanager.hpp
      ${CMAKE_SOURCE_DIR}/src/scenemanager.cpp
      ${CMAKE_SOURCE_DIR}/include/scenemanager.hpp
      ${CMAKE_SOURCE_DIR}/src/scene3d.cpp
      ${CMAKE_SOURCE_DIR}/include/scene3d.hpp
  )

  target_compile_definitions(CleanEngineGL PRIVATE GLM_FORCE_RADIANS)
  target_compile_definitions(CleanEngineGL PRIVATE RENDERER_OPENGL)

  target_include_directories(CleanEngineGL PUBLIC
    ${CMAKE_SOURCE_DIR}/include
  )

  find_package(assimp REQUIRED)
  target_link_libraries(CleanEngineGL assimp::assimp)

  if(USE_IMGUI)
    find_package(imgui REQUIRED)
    target_include_directories(CleanEngineGL PRIVATE
      ${IMGUI_INCLUDE_DIR}
    )
    target_link_libraries(CleanEngineGL freetype imgui::imgui)

    target_sources(CleanEngineGL PRIVATE
      ${CMAKE_SOURCE_DIR}/libs/imgui_backends/src/imgui_impl_opengl3.cpp
      ${CMAKE_SOURCE_DIR}/libs/imgui_backends/include/imgui_impl_opengl3.h
      ${CMAKE_SOURCE_DIR}/libs/imgui_backends/include/imgui_impl_opengl3_loader.h
    )
    if(USE_GLFW)
      target_sources(CleanEngineGL PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/imgui_backends/src/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/libs/imgui_backends/include/imgui_impl_glfw.h
      )
    endif()

    target_include_directories(CleanEngineGL PRIVATE
      ${CMAKE_SOURCE_DIR}/libs/imgui_backends/include
    )
  endif()

  if(USE_GLFW)
    find_package(glfw3 REQUIRED)
    target_link_libraries(CleanEngineGL glfw)

    target_sources(CleanEngineGL PUBLIC
      ${CMAKE_SOURCE_DIR}/src/enginecoreglfw.cpp
      ${CMAKE_SOURCE_DIR}/include/enginecoreglfw.hpp
    )
    target_compile_definitions(CleanEngineGL PRIVATE CORE_GLFW)
  endif()

  if(USE_BULLET)
    find_package(Bullet REQUIRED)
    target_include_directories(CleanEngineGL PRIVATE
      ${BULLET_INCLUDE_DIRS}
    )
    target_link_libraries(CleanEngineGL
      ${BULLET_LIBRARIES}
    )

    if(BULLET_MULTITHREADED)
      find_package(OpenMP REQUIRED)
      target_include_directories(CleanEngineGL PRIVATE
        ${OpenMP_CXX_INCLUDE_DIRS}
      )
      target_link_libraries(CleanEngineGL ${OpenMP_CXX_LIBRARIES})
    endif()
  endif()

  if(MSVC)
    add_compile_options("/std:c++20")
    add_compile_definitions(NOMINMAX)
    # TODO
  elseif(UNIX)
    target_include_directories(CleanEngineGL PRIVATE
      ${CMAKE_SOURCE_DIR}/libs/fmodstudioapi20206linux/api/core/inc/
    )

    target_link_directories(CleanEngineGL PRIVATE
      ${CMAKE_SOURCE_DIR}/libs/fmodstudioapi20206linux/api/core/lib/x86_64/
    )

    target_link_libraries(CleanEngineGL -lGLEW -lGL -ldl -lpthread -lIL -lILU -llua)
    target_link_libraries(CleanEngineGL -lfmod) # Audio
  endif()
endif()
