# Minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(heap_allocator LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE HEAP_SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Executable: main heap allocator
add_executable(heap_main
    ${PROJECT_SOURCE_DIR}/src/main.c
    ${HEAP_SOURCES}
)

# Compiler warnings
if(MSVC)
    target_compile_options(heap_main PRIVATE /W4 /permissive-)
else()
    target_compile_options(heap_main PRIVATE -Wall -Wextra -pedantic)
endif()

# Optional: Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/Release)