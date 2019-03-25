cmake_minimum_required(VERSION 3.13)

project(Computer-Graphics)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR})

add_executable(bezier 01_bezier_curves/bezier.cpp)
target_link_libraries(bezier ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY})