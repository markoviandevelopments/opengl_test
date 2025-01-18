cmake_minimum_required(VERSION 3.10)
project(ChessboardWorld)

set(CMAKE_CXX_STANDARD 17)

# Add include directory for GLAD
include_directories(${CMAKE_SOURCE_DIR}/include)

# Specify source files explicitly
set(SOURCES
    main.cpp
    src/glad/glad.c
)

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Create the executable
add_executable(ChessboardWorld ${SOURCES})

# Link the required libraries
target_link_libraries(ChessboardWorld OpenGL::GL glfw)
