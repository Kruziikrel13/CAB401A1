cmake_minimum_required(VERSION 3.30)

# Get version
file(READ "${CMAKE_SOURCE_DIR}/VERSION" VER_RAW)
string(STRIP ${VER_RAW} VER)

project(
  "CAB401 Assignment 1" 
  DESCRIPTION "QUT CAB401 Assignment 1 Code"
  VERSION ${VER}
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
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

include_directories(include/) 

# Sequential Prog
add_executable(sequential src/sequential.cpp)

# Vulkan Prog
add_executable(vulkan
  src/vulkan.cpp
  src/VulkanContext.cpp)

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DEBUG)
  message(STATUS "Configuring ${PROJECT_NAME} in Debug with CMake")

  if(WITH_ASAN)
    message(STATUS "Enabling ASan")

    target_link_libraries(vulkan asan)
    target_compile_options(vulkan PUBLIC -fsanitize=address)
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

find_package(spdlog REQUIRED)
target_link_libraries(vulkan spdlog::spdlog)
