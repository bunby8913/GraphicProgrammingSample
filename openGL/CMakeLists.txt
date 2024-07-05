
cmake_minimum_required(VERSION 3.10)

project(learnOpenGl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set policy CMP0072 to prefer GLVND
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../GLFW/glfw/include ${CMAKE_CURRENT_SOURCE_DIR}/../assimp/include ${CMAKE_SOURCE_DIR}/include/glm)

# Link directories
link_directories(${CMAKE_SOURCE_DIR}/../GLFW/glfw/build/src ${CMAKE_SOURCE_DIR}../assimp/bin/)
# Define the path to the Assimp library
# Find OpenGL package
find_package(OpenGL REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/glad.c")

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})
# Link libraries
target_link_libraries(${PROJECT_NAME} glfw3 ${OPENGL_LIBRARIES} ${CMAKE_SOURCE_DIR}/../assimp/bin/libassimp.so X11 pthread Xrandr Xi dl)
