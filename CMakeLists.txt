cmake_minimum_required(VERSION 3.16)

project(playground)

# Dependencies
set(XGFX_API DIRECTX12 CACHE STRING "Which graphics API to use")
set_property(CACHE XGFX_API PROPERTY STRINGS DIRECTX12)

set(IMGUI_SOURCES
    3rdparty/imgui/imgui.cpp
    3rdparty/imgui/imgui_draw.cpp
    3rdparty/imgui/imgui_demo.cpp
    3rdparty/imgui/imgui_widgets.cpp
    3rdparty/imgui/imgui_tables.cpp
    3rdparty/imgui/backends/imgui_impl_win32.cpp
    3rdparty/imgui/backends/imgui_impl_dx12.cpp)
    
add_library(imgui STATIC ${IMGUI_SOURCES})
add_subdirectory(3rdparty/crosswindow)
add_subdirectory(3rdparty/crosswindow-graphics)
add_subdirectory(3rdparty/fast_obj)
add_subdirectory(3rdparty/meshoptimizer)

set(IMGUI_DIR 3rdparty/imgui)

set_property(TARGET imgui PROPERTY FOLDER "3rdparty")

# Executable
file(GLOB_RECURSE SRC_FILES "src/*.h" "src/*.hpp" "src/*.cpp")
xwin_add_executable(${PROJECT_NAME} "${SRC_FILES}")

file(COPY "src/shaders" DESTINATION "${PROJECT_BINARY_DIR}/src")
file(COPY "3rdparty/imgui/misc/fonts" DESTINATION "${PROJECT_BINARY_DIR}/3rdparty/imgui/misc")

# Finish dependencies
target_link_libraries(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} CrossWindowGraphics)
target_link_libraries(${PROJECT_NAME} CrossWindow)
target_link_libraries(${PROJECT_NAME} fast_obj_lib)
target_link_libraries(${PROJECT_NAME} meshoptimizer)

target_include_directories(imgui PRIVATE 3rdparty/imgui)
target_include_directories(${PROJECT_NAME} PUBLIC 3rdparty/glm)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/imgui/backends)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/fast_obj)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/meshoptimizer/src)

add_dependencies(${PROJECT_NAME} CrossWindowGraphics CrossWindow)

target_compile_definitions(${PROJECT_NAME} PUBLIC XGFX_${XGFX_API}=1)

# Project settings
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG_>)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:RELEASE_>)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)
