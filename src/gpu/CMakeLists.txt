add_library(LibGPU STATIC)



if(GRAPHICS_API STREQUAL "Vulkan")
    list(APPEND LIBGPU_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/vulkan/VulkanContext.h)
    list(APPEND LIBGPU_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/vulkan/VulkanContext.cpp)
    source_group("Vulkan" FILES ${LIBGPU_HEADERS} ${LIBGPU_SOURCES})
elseif(GRAPHICS_API STREQUAL "DX12")
    list(APPEND LIBGPU_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/dx12/DX12Context.h)
    list(APPEND LIBGPU_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/dx12/DX12Context.cpp)
    source_group("DX12" FILES ${LIBGPU_HEADERS} ${LIBGPU_SOURCES})
endif()

list(APPEND LIBGPU_HEADERS 
    Context.h
    IndexBuffer.h
    TextureBuffer.h
    VertexBuffer.h)

target_sources(LibGPU PRIVATE ${LIBGPU_HEADERS} ${LIBGPU_SOURCES})
target_include_directories(LibGPU PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})