cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Compiled Version and the usage
# 0 - default (pretty much no usage) - can me commented out
# 1 - iRacing
# 2 - Secret
set(COMPILE_TYPE_VALUE 1)

project(KevS VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(VULKAN_SDK "S:/Vulkan")
set(GLFW_DIR "glfw/CMake") 

add_subdirectory(irsdk)
add_subdirectory(imgui)
add_subdirectory(glfw)

add_definitions(-DKEVS_USAGE_TYPE=${COMPILE_TYPE_VALUE})

include_directories("glfw/include/GLFW")
include_directories("${VULKAN_SDK}/Include")
include_directories(include)
include_directories(irsdk)
include_directories(imgui)
include_directories(imspinner)

link_directories("${VULKAN_SDK}/Lib")

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)
#find_library(VULKAN_LIBRARY vulkan-1)

add_executable(KevS ${SOURCES} ${HEADERS} imspinner/imspinner.h)
target_link_libraries(KevS PRIVATE IRSDK_LIB)
target_link_libraries(KevS PRIVATE IMGUI_LIB)
target_link_libraries(KevS PRIVATE d3d11 dxgi d3dcompiler dwmapi)
target_link_libraries(KevS PRIVATE "${VULKAN_SDK}/Lib/vulkan-1.lib")
target_link_libraries(KevS PRIVATE glfw)

if (WIN32)
	set_target_properties(KevS PROPERTIES WIN32_EXECUTABLE YES) # Entfernt Konsole
endif()