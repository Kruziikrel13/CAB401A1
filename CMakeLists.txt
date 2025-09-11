cmake_minimum_required(VERSION 3.10)

# Get version
file(READ "${CMAKE_SOURCE_DIR}/VERSION" VER_RAW)
string(STRIP ${VER_RAW} VER)

project(
  CPP_TEMPLATE
  DESCRIPTION "C++ Template Repository"
  VERSION ${VER}
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
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

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DEBUG)
  message(STATUS "Configuring ${PROJECT_NAME} in Debug with CMake")

  if(WITH_ASAN)
    message(STATUS "Enabling ASan")

    target_link_libraries(${TARGETS})
    target_compile_options(${TARGETS} PUBLIC -fsanitize=address)
  endif()

  add_compile_options(-fno-pie -fno-builtin)
  add_link_options(-no-pie -fno-builtin)
else()
  add_compile_options(-O3)
  message(STATUS "Configuring ${PROJECT_NAME} in Release with CMake")
endif()

include_directories(. "src/")
