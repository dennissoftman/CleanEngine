cmake_minimum_required(VERSION 3.10)

project(CleanEngine LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -Og -ggdb")

option(USE_OPENGL "Compile using OpenGL renderer" OFF)
option(USE_VULKAN "Compile using Vulkan renderer" ON)

option(USE_BULLET "Compile using Bullet physics engien" ON)
option(BULLET_MULTITHREADED "Compile using multithreaded Bullet3 (experimental)" OFF)

option(USE_GLFW "Use GLFW library" ON)

option(USE_IMGUI "Use bloat-free imgui library" ON)
option(USE_CEGUI "Use Crazy Eddie's GUI library" OFF)

option(USE_FMOD "Use FMODex as audio backend" OFF) # by default use DummyAudioManager (no sound)

option(USE_LUA "Use Lua script backend" ON)

option(USE_STEAM "Enable Steam API" OFF)

# default path for windows
set(FMOD_DIR "C:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows" CACHE PATH "Path to FMOD SDK")

set(COMMON_SOURCES
  ${CMAKE_SOURCE_DIR}/include/renderer.hpp
  ${CMAKE_SOURCE_DIR}/src/dummyrenderer.cpp
  ${CMAKE_SOURCE_DIR}/include/dummyrenderer.hpp
  ${CMAKE_SOURCE_DIR}/include/shader.hpp
  ${CMAKE_SOURCE_DIR}/src/logger.cpp
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
  ${CMAKE_SOURCE_DIR}/src/imageloader.cpp
  ${CMAKE_SOURCE_DIR}/include/imageloader.hpp
  ${CMAKE_SOURCE_DIR}/src/material.cpp
  ${CMAKE_SOURCE_DIR}/include/material.hpp
  ${CMAKE_SOURCE_DIR}/src/materialmanager.cpp
  ${CMAKE_SOURCE_DIR}/include/materialmanager.hpp
  ${CMAKE_SOURCE_DIR}/src/modelmanager.cpp
  ${CMAKE_SOURCE_DIR}/include/modelmanager.hpp
  ${CMAKE_SOURCE_DIR}/include/scriptengine.hpp
  ${CMAKE_SOURCE_DIR}/src/dummyscriptengine.cpp
  ${CMAKE_SOURCE_DIR}/include/dummyscriptengine.hpp
  ${CMAKE_SOURCE_DIR}/include/audiomanager.hpp
  ${CMAKE_SOURCE_DIR}/src/dummyaudiomanager.cpp
  ${CMAKE_SOURCE_DIR}/include/dummyaudiomanager.hpp
  ${CMAKE_SOURCE_DIR}/src/scenemanager.cpp
  ${CMAKE_SOURCE_DIR}/include/scenemanager.hpp
  ${CMAKE_SOURCE_DIR}/src/entity.cpp
  ${CMAKE_SOURCE_DIR}/include/entity.hpp
  ${CMAKE_SOURCE_DIR}/src/staticmesh.cpp
  ${CMAKE_SOURCE_DIR}/include/staticmesh.hpp
  ${CMAKE_SOURCE_DIR}/src/scene3d.cpp
  ${CMAKE_SOURCE_DIR}/include/scene3d.hpp
  ${CMAKE_SOURCE_DIR}/include/physicsmanager.hpp
  ${CMAKE_SOURCE_DIR}/src/dummyphysicsmanager.cpp
  ${CMAKE_SOURCE_DIR}/include/dummyphysicsmanager.hpp
  ${CMAKE_SOURCE_DIR}/src/resourcemanager.cpp
  ${CMAKE_SOURCE_DIR}/include/resourcemanager.hpp
  ${CMAKE_SOURCE_DIR}/src/uimanager.cpp
  ${CMAKE_SOURCE_DIR}/include/uimanager.hpp
  ${CMAKE_SOURCE_DIR}/src/dummyuimanager.cpp
  ${CMAKE_SOURCE_DIR}/include/dummyuimanager.hpp
)

if(USE_VULKAN)
  add_executable(CleanEngineVk
    ${CMAKE_SOURCE_DIR}/main.cpp
  )

  target_sources(CleanEngineVk PRIVATE
    ${COMMON_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/vulkanrenderer.cpp
    ${CMAKE_SOURCE_DIR}/include/vulkanrenderer.hpp
    ${CMAKE_SOURCE_DIR}/src/vkshader.cpp
    ${CMAKE_SOURCE_DIR}/include/vkshader.hpp
    ${CMAKE_SOURCE_DIR}/src/vkmaterial.cpp
    ${CMAKE_SOURCE_DIR}/include/vkmaterial.hpp
  )

  if(USE_FMOD)
    target_compile_definitions(CleanEngineVk PRIVATE AUDIO_FMOD)
    target_sources(CleanEngineVk PRIVATE
      ${CMAKE_SOURCE_DIR}/src/fmodaudiomanager.cpp
      ${CMAKE_SOURCE_DIR}/include/fmodaudiomanager.hpp
    )
  endif()

  target_compile_definitions(CleanEngineVk PRIVATE GLM_FORCE_RADIANS)
  target_compile_definitions(CleanEngineVk PRIVATE RENDERER_VULKAN)

  target_include_directories(CleanEngineVk PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs
  )

  # toml++
  find_package(tomlplusplus REQUIRED)
  target_link_libraries(CleanEngineVk tomlplusplus::tomlplusplus)

  # GLM
  find_package(glm REQUIRED)
  target_compile_definitions(CleanEngineVk PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE) # depth fix
  target_include_directories(CleanEngineVk PRIVATE ${GLM_INCLUDE_DIR})
  target_link_libraries(CleanEngineVk glm::glm)

  # Vulkan
  find_package(Vulkan REQUIRED)
  target_include_directories(CleanEngineVk PRIVATE Vulkan::Headers)
  target_link_libraries(CleanEngineVk Vulkan::Vulkan)

  find_package(minizip REQUIRED)
  target_link_libraries(CleanEngineVk minizip::minizip)

  # SAIL
  find_package(Sail CONFIG REQUIRED)
  target_link_libraries(CleanEngineVk SAIL::sail sail-manip)

  # minizip
  find_package(minizip REQUIRED)
  target_link_libraries(CleanEngineVk minizip::minizip)

  # Assimp
  find_package(assimp REQUIRED)
  target_link_libraries(CleanEngineVk assimp::assimp)

  if(USE_IMGUI)
    target_sources(CleanEngineVk PRIVATE
      ${CMAKE_SOURCE_DIR}/src/imguivkuimanager.cpp
      ${CMAKE_SOURCE_DIR}/include/imguivkuimanager.hpp
    )

    find_package(Freetype REQUIRED)
    target_link_libraries(CleanEngineVk ${FREETYPE_LIBRARIES})

    find_package(imgui REQUIRED)
    target_include_directories(CleanEngineVk PRIVATE
      ${IMGUI_INCLUDE_DIR}
    )
    target_link_libraries(CleanEngineVk imgui::imgui)

    target_sources(CleanEngineVk PRIVATE
      ${CMAKE_SOURCE_DIR}/libs/imgui_backends/src/imgui_impl_vulkan.cpp
      ${CMAKE_SOURCE_DIR}/libs/imgui_backends/include/imgui_impl_vulkan.h
    )
    if(USE_GLFW)
      target_sources(CleanEngineVk PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/imgui_backends/src/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/libs/imgui_backends/include/imgui_impl_glfw.h
      )
    endif()

    target_include_directories(CleanEngineVk PRIVATE
      ${CMAKE_SOURCE_DIR}/libs/imgui_backends/include
    )
    target_compile_definitions(CleanEngineVk PRIVATE UI_IMGUI)
  endif()

  if(USE_GLFW)
    find_package(glfw3 REQUIRED)
    target_link_libraries(CleanEngineVk glfw)

    target_sources(CleanEngineVk PUBLIC
      ${CMAKE_SOURCE_DIR}/src/enginecoreglfw.cpp
      ${CMAKE_SOURCE_DIR}/include/enginecoreglfw.hpp
    )
    target_compile_definitions(CleanEngineVk PRIVATE CORE_GLFW)
  endif()

  if(USE_BULLET)
    find_package(Bullet REQUIRED)
    target_include_directories(CleanEngineVk PRIVATE
      ${BULLET_INCLUDE_DIRS}
    )
    target_link_libraries(CleanEngineVk
      ${BULLET_LIBRARIES}
    )

    find_package(OpenMP REQUIRED)
    target_include_directories(CleanEngineVk PRIVATE
      ${OpenMP_CXX_INCLUDE_DIRS}
    )
    target_link_libraries(CleanEngineVk ${OpenMP_CXX_LIBRARIES})

    if(BULLET_MULTITHREADED)
      target_compile_definitions(CleanEngineVk PRIVATE BULLET_MULTITHREADED)
    endif()

    target_sources(CleanEngineVk PRIVATE
      ${CMAKE_SOURCE_DIR}/src/bulletphysicsmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/bulletphysicsmanager.hpp
    )
    target_compile_definitions(CleanEngineVk PRIVATE PHYSICS_BULLET)
  endif()

  if(USE_LUA)
    target_compile_definitions(CleanEngineVk PRIVATE SCRIPT_ENGINE_LUA)
    target_sources(CleanEngineVk PRIVATE
      ${CMAKE_SOURCE_DIR}/src/luascriptengine.cpp
      ${CMAKE_SOURCE_DIR}/include/luascriptengine.hpp
    )

    find_package(Lua REQUIRED)
    target_include_directories(CleanEngineVk PRIVATE ${LUA_INCLUDE_DIR})
    target_link_libraries(CleanEngineVk ${LUA_LIBRARIES})
  endif()

  if(MSVC)
    set_target_properties(CleanEngineVk PROPERTIES LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
    add_compile_options(/std:c++20 /arch:AVX2)
    add_compile_definitions(NOMINMAX)
    # Audio
    if(USE_FMOD)
      target_include_directories(CleanEngineVk PRIVATE ${FMOD_DIR}/api/core/inc)
      target_link_directories(CleanEngineVk PRIVATE ${FMOD_DIR}/api/core/lib/x64)
      target_link_libraries(CleanEngineVk fmod_vc.lib)
    endif()

    if(USE_STEAM)
      target_include_directories(CleanEngineVk PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/steamworks_sdk/public
      )
      target_link_directories(CleanEngineVk PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/steamworks_sdk/redistributable_bin/win64/
      )
      target_link_libraries(CleanEngineVk steam_api64.lib)
    endif()

  elseif(UNIX)
    set(CMAKE_CXX_FLAGS "-mavx2")

    find_package(X11 REQUIRED)
    target_link_libraries(CleanEngineVk ${X11_LIBRARIES} X11::X11_xcb -ldl -lpthread)

    if(USE_FMOD)
      target_include_directories(CleanEngineVk PRIVATE
        ${FMOD_DIR}/api/core/inc/
      )
      target_link_directories(CleanEngineVk PRIVATE
        ${FMOD_DIR}/api/core/lib/x86_64/
      )
      target_link_libraries(CleanEngineVk -lfmod) # Audio
    endif()

    if(USE_STEAM)
      target_include_directories(CleanEngineVk PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/steamworks_sdk/public
      )
      target_link_directories(CleanEngineVk PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/steamworks_sdk/redistributable_bin/linux64/
      )
      target_link_libraries(CleanEngineVk -lsteam_api)
    endif()
  endif()
elseif(USE_OPENGL)
  message(SEND_ERROR "Sorry, deprecated!")
endif()
