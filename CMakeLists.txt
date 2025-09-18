cmake_minimum_required(VERSION 3.30)

# Get version
file(READ "${CMAKE_SOURCE_DIR}/VERSION" VER_RAW)
string(STRIP ${VER_RAW} VER)

project(
  "CAB401 Assignment 1"
  DESCRIPTION "QUT CAB401 Assignment 1 Code"
  VERSION ${VER}
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(
  -Wall
  -Wextra
  -Wpedantic
  -Wno-unused-parameter
  -Wno-unused-value
  -Wno-missing-field-initializers
  -Wno-gnu-zero-variadic-macro-arguments
  -Wno-narrowing
  -Wno-pointer-arith
  -Wno-clobbered
  -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=)

set(CMAKE_EXECUTABLE_ENABLE_EXPORTS TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)
add_library(common STATIC
  src/common.cpp)
target_include_directories(common PUBLIC include)

find_package(spdlog REQUIRED)

# Sequential Prog
add_executable(cpu src/cpu.cpp)
target_link_libraries(cpu PRIVATE common)
target_link_libraries(cpu PRIVATE spdlog::spdlog)

# Vulkan Prog
add_executable(vulkan
  src/vulkan.cpp
  src/VulkanContext.cpp)

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DEBUG)
  message(STATUS "Configuring ${PROJECT_NAME} in Debug with CMake")

  if(TRACE OR WITH_ASAN)
    target_compile_definitions(vulkan PRIVATE -DTRACE)
  endif()

  if(WITH_ASAN)
    message(STATUS "Enabling ASan")

    target_link_libraries(vulkan asan)
    target_compile_options(vulkan PRIVATE -fsanitize=address)
  endif()

  add_compile_options(-fno-pie -fno-builtin)
  add_link_options(-no-pie -fno-builtin)
else()
  add_compile_options(-O3)
  message(STATUS "Configuring ${PROJECT_NAME} in Release with CMake")
endif()

find_package(Vulkan REQUIRED)
target_include_directories(vulkan PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vulkan ${Vulkan_LIBRARIES})
target_compile_definitions(vulkan PRIVATE
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)
target_link_libraries(vulkan PRIVATE spdlog::spdlog)
