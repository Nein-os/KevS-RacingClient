cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(IMGUI_LIB VERSION 1.90.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/.h")

include_directories("../glfw/include/GLFW")
include_directories("S:/Vulkan/Include")

find_package(Vulkan REQUIRED)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})