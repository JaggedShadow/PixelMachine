set(APP_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

source_group("Application" FILES ${APP_SRCS})
add_executable(PixelMachine ${APP_SRCS})
target_link_libraries(PixelMachine PUBLIC LibGPU)
target_include_directories(PixelMachine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../)