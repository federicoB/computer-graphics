cmake_minimum_required(VERSION 3.10)

project(Computer-Graphics)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR} ${glm_INCLUDE_DIRS})

add_executable(bezier 01_bezier_curves/bezier.cpp)
target_link_libraries(bezier ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY})

add_executable(model_viewer 03_model_viewer/main_students.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY. 03_model_viewer)
target_link_libraries(model_viewer ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY} glm)

add_executable(shaders 06_shaders/main_students.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 06_shaders)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive")
target_link_libraries(shaders ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY} glm)