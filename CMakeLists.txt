cmake_minimum_required(VERSION 3.13)

project(Computer-Graphics)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR} ${glm_INCLUDE_DIRS})

add_executable(01_bezier_curves 01_bezier_curves/bezier.cpp)
target_link_libraries(01_bezier_curves ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY})


add_executable(03_model_viewer 03_model_viewer/main_students.cpp)
target_link_libraries(03_model_viewer ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY} glm)

include_directories(06_shaders)
add_executable(06_shaders 06_shaders/main_students.cpp)
target_link_libraries(06_shaders ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY} glm)