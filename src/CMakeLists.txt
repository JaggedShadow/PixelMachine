cmake_minimum_required(VERSION 4.1.1 FATAL_ERROR)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_CXX_STANDARD 20)

set(GRAPHICS_API "Vulkan" CACHE STRING "Select graphics API for a renderer")
set_property(CACHE GRAPHICS_API PROPERTY STRINGS "Vulkan" "DX12")

project(PixelMachine)

add_subdirectory("app")
add_subdirectory("gpu")