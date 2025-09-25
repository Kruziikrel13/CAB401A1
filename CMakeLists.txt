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
add_executable(sequential src/sequential.cpp)
target_link_libraries(sequential PRIVATE common)
target_link_libraries(sequential PRIVATE spdlog::spdlog)

#  OpenMP Prog
find_package(OpenMP)
add_executable(omp src/omp.cpp)
target_link_libraries(omp PRIVATE common)
target_link_libraries(omp PRIVATE spdlog::spdlog)
target_link_libraries(omp PRIVATE OpenMP::OpenMP_CXX)


# Vulkan Prog
add_executable(vulkan
  src/vulkan.cpp
  src/VulkanContext.cpp)
target_link_libraries(vulkan PRIVATE common)

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DEBUG)
  message(STATUS "Configuring ${PROJECT_NAME} in Debug with CMake")

  if(TRACE OR WITH_ASAN)
    target_compile_definitions(sequential PRIVATE -DTRACE)
    target_compile_definitions(vulkan PRIVATE -DTRACE)
    target_compile_options(sequential PRIVATE -O0 -g)
    target_compile_options(vulkan PRIVATE -O0 -g)
  endif()

  if(WITH_ASAN)
    message(STATUS "Enabling ASan")

    target_link_libraries(sequential PRIVATE asan)
    target_compile_options(sequential PRIVATE -fsanitize=address)

    target_link_libraries(vulkan PRIVATE asan)
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
target_link_libraries(vulkan PRIVATE ${Vulkan_LIBRARIES})
target_compile_definitions(vulkan PRIVATE
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)
target_link_libraries(vulkan PRIVATE spdlog::spdlog)
