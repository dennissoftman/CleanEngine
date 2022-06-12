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
  ${CMAKE_SOURCE_DIR}/include/client/renderer.hpp
  ${CMAKE_SOURCE_DIR}/src/client/dummyrenderer.cpp
  ${CMAKE_SOURCE_DIR}/include/client/dummyrenderer.hpp
  ${CMAKE_SOURCE_DIR}/include/client/shader.hpp
  ${CMAKE_SOURCE_DIR}/src/common/logger.cpp
  ${CMAKE_SOURCE_DIR}/include/common/logger.hpp
  ${CMAKE_SOURCE_DIR}/src/common/dummylogger.cpp
  ${CMAKE_SOURCE_DIR}/include/common/dummylogger.hpp
  ${CMAKE_SOURCE_DIR}/src/common/debuglogger.cpp
  ${CMAKE_SOURCE_DIR}/include/common/debuglogger.hpp
  ${CMAKE_SOURCE_DIR}/src/common/servicelocator.cpp
  ${CMAKE_SOURCE_DIR}/include/common/servicelocator.hpp
  ${CMAKE_SOURCE_DIR}/src/common/eventmanager.cpp
  ${CMAKE_SOURCE_DIR}/include/common/eventmanager.hpp
  ${CMAKE_SOURCE_DIR}/src/common/entities/camera3d.cpp
  ${CMAKE_SOURCE_DIR}/include/common/entities/camera3d.hpp
  ${CMAKE_SOURCE_DIR}/src/client/imageloader.cpp
  ${CMAKE_SOURCE_DIR}/include/client/imageloader.hpp
  ${CMAKE_SOURCE_DIR}/include/client/material.hpp
  ${CMAKE_SOURCE_DIR}/src/client/materialmanager.cpp
  ${CMAKE_SOURCE_DIR}/include/client/materialmanager.hpp
  ${CMAKE_SOURCE_DIR}/src/common/modelmanager.cpp
  ${CMAKE_SOURCE_DIR}/include/common/modelmanager.hpp
  ${CMAKE_SOURCE_DIR}/include/common/scriptengine.hpp
  ${CMAKE_SOURCE_DIR}/src/common/dummyscriptengine.cpp
  ${CMAKE_SOURCE_DIR}/include/common/dummyscriptengine.hpp
  ${CMAKE_SOURCE_DIR}/include/client/audiomanager.hpp
  ${CMAKE_SOURCE_DIR}/src/client/dummyaudiomanager.cpp
  ${CMAKE_SOURCE_DIR}/include/client/dummyaudiomanager.hpp
  ${CMAKE_SOURCE_DIR}/src/server/scenemanager.cpp
  ${CMAKE_SOURCE_DIR}/include/server/scenemanager.hpp
  ${CMAKE_SOURCE_DIR}/include/common/entities/entity.hpp
  ${CMAKE_SOURCE_DIR}/src/common/entities/staticmesh.cpp
  ${CMAKE_SOURCE_DIR}/include/common/entities/staticmesh.hpp
  ${CMAKE_SOURCE_DIR}/src/server/scene3d.cpp
  ${CMAKE_SOURCE_DIR}/include/server/scene3d.hpp
  ${CMAKE_SOURCE_DIR}/include/server/physicsmanager.hpp
  ${CMAKE_SOURCE_DIR}/src/server/dummyphysicsmanager.cpp
  ${CMAKE_SOURCE_DIR}/include/server/dummyphysicsmanager.hpp
  ${CMAKE_SOURCE_DIR}/src/common/resourcemanager.cpp
  ${CMAKE_SOURCE_DIR}/include/common/resourcemanager.hpp
  ${CMAKE_SOURCE_DIR}/include/client/uimanager.hpp
  ${CMAKE_SOURCE_DIR}/src/client/dummyuimanager.cpp
  ${CMAKE_SOURCE_DIR}/include/client/dummyuimanager.hpp
  ${CMAKE_SOURCE_DIR}/include/common/gameservices.hpp
  ${CMAKE_SOURCE_DIR}/src/common/dummyservices.cpp
  ${CMAKE_SOURCE_DIR}/include/common/dummyservices.hpp
  ${CMAKE_SOURCE_DIR}/include/common/dirty_flag.hpp
)

if(USE_VULKAN)
  add_executable(CleanEngineVk
    ${CMAKE_SOURCE_DIR}/main.cpp
  )

  target_sources(CleanEngineVk PRIVATE
    ${COMMON_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/client/vulkanrenderer.cpp
    ${CMAKE_SOURCE_DIR}/include/client/vulkanrenderer.hpp
    ${CMAKE_SOURCE_DIR}/src/client/vkshader.cpp
    ${CMAKE_SOURCE_DIR}/include/client/vkshader.hpp
    ${CMAKE_SOURCE_DIR}/include/client/shaders/vk_color_shader.hpp
    ${CMAKE_SOURCE_DIR}/src/client/shaders/vk_color_shader.cpp
    ${CMAKE_SOURCE_DIR}/src/client/shaders/vk_image_shader.cpp
    ${CMAKE_SOURCE_DIR}/include/client/shaders/vk_image_shader.hpp
    ${CMAKE_SOURCE_DIR}/src/client/vkmaterial.cpp
    ${CMAKE_SOURCE_DIR}/include/client/vkmaterial.hpp
  )

  if(USE_FMOD)
    target_compile_definitions(CleanEngineVk PRIVATE AUDIO_FMOD)
    target_sources(CleanEngineVk PRIVATE
      ${CMAKE_SOURCE_DIR}/src/client/fmodaudiomanager.cpp
      ${CMAKE_SOURCE_DIR}/include/client/fmodaudiomanager.hpp
    )
  endif()

  target_compile_definitions(CleanEngineVk PRIVATE GLM_FORCE_RADIANS)
  target_compile_definitions(CleanEngineVk PRIVATE RENDERER_VULKAN)

  target_include_directories(CleanEngineVk PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs
  )

  # boost
  find_package(Boost REQUIRED)
  target_link_libraries(CleanEngineVk Boost::boost)

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

  # minizip
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
      ${CMAKE_SOURCE_DIR}/src/client/imguivkuimanager.cpp
      ${CMAKE_SOURCE_DIR}/include/client/imguivkuimanager.hpp
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
      ${CMAKE_SOURCE_DIR}/src/client/gameclientglfw.cpp
      ${CMAKE_SOURCE_DIR}/include/client/gameclientglfw.hpp
    )
    target_compile_definitions(CleanEngineVk PRIVATE CLIENT_GLFW)
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
      ${CMAKE_SOURCE_DIR}/src/server/bulletphysicsmanager.cpp
      ${CMAKE_SOURCE_DIR}/include/server/bulletphysicsmanager.hpp
    )
    target_compile_definitions(CleanEngineVk PRIVATE PHYSICS_BULLET)
  endif()

  if(USE_LUA)
    target_compile_definitions(CleanEngineVk PRIVATE SCRIPT_ENGINE_LUA)
    target_sources(CleanEngineVk PRIVATE
      ${CMAKE_SOURCE_DIR}/src/common/luascriptengine.cpp
      ${CMAKE_SOURCE_DIR}/include/common/luascriptengine.hpp
    )

    find_package(Lua REQUIRED)
    target_include_directories(CleanEngineVk PRIVATE ${LUA_INCLUDE_DIR})
    target_link_libraries(CleanEngineVk ${LUA_LIBRARIES})

    find_package(sol2 REQUIRED)
    target_link_libraries(CleanEngineVk sol2::sol2)
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
      target_sources(CleanEngineVk PRIVATE
        ${CMAKE_SOURCE_DIR}/src/common/steamservices.cpp
        ${CMAKE_SOURCE_DIR}/include/common/steamservices.hpp
      )
      target_include_directories(CleanEngineVk PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/steamworks_sdk/public
      )
      target_link_directories(CleanEngineVk PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/steamworks_sdk/redistributable_bin/linux64/
      )
      target_link_libraries(CleanEngineVk -lsteam_api)
      target_compile_definitions(CleanEngineVk PRIVATE SERVICES_STEAM)
    endif()
  endif()
elseif(USE_OPENGL)
  message(SEND_ERROR "Sorry, deprecated!")
endif()
