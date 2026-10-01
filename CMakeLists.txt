# Copyright (c) 2026 AIperture-Labs & Xavier Beheydt <xavier.beheydt@gmail.com>

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(FetchContent)


project(HowToVulkan VERSION 2026 LANGUAGES C CXX)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
elseif(LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WAYLAND_KHR")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")
if(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")


find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_path(SLANG_INCLUDE_DIR slang.h REQUIRED)
find_library(SLANG_LIBRARY slang REQUIRED)

include_directories(extern/tinyobj)
include_directories(extern/ktx/include)
include_directories(extern/ktx/other_include)

set(KTX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/ktx)
set(KTX_SOURCES
    ${KTX_DIR}/lib/texture.c
    ${KTX_DIR}/lib/hashlist.c
    ${KTX_DIR}/lib/checkheader.c
    ${KTX_DIR}/lib/swap.c
    ${KTX_DIR}/lib/memstream.c
    ${KTX_DIR}/lib/filestream.c
    ${KTX_DIR}/lib/vkloader.c)
add_library(ktx STATIC ${KTX_SOURCES})

if (TARGET Vulkan::Vulkan)
    target_link_libraries(ktx PUBLIC Vulkan::Vulkan)
elseif (APPLE)
    target_link_libraries(ktx PUBLIC $ENV{VULKAN_SDK}/lib/libvulkan.dylib)
elseif (NOT WIN32)
    target_link_libraries(ktx PUBLIC $ENV{VULKAN_SDK}/lib/libvulkan.so)
endif()

## Set the main executable
add_executable(how-to-vulkan src/main.cpp)
target_compile_definitions(how-to-vulkan PRIVATE VK_NO_PROTOTYPES)
if (WIN32)
    target_compile_definitions(how-to-vulkan PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
set_target_properties(how-to-vulkan PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(how-to-vulkan PRIVATE cxx_std_20)
target_link_libraries(how-to-vulkan PRIVATE SDL3::SDL3 ktx ${SLANG_LIBRARY})
if (NOT WIN32)
    target_link_libraries(how-to-vulkan PRIVATE glm::glm-header-only)
endif()
