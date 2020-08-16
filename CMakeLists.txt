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


add_executable(03_model_viewer 03_model_viewer/main_students.cpp 03_model_viewer/HUD_Logger.cpp)
target_link_libraries(03_model_viewer ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY} glm)


include_directories(05_ray_tracing)
add_executable(05_ray_tracing 05_ray_tracing/main.cpp 05_ray_tracing/boundingbox.cpp 05_ray_tracing/camera.cpp 05_ray_tracing/edge.cpp
        05_ray_tracing/face.cpp 05_ray_tracing/glCanvas.cpp 05_ray_tracing/material.cpp 05_ray_tracing/matrix.cpp 05_ray_tracing/mesh.cpp
        05_ray_tracing/radiosity.cpp 05_ray_tracing/raytracer_students.cpp 05_ray_tracing/raytree.cpp 05_ray_tracing/sphere.cpp)
target_link_libraries(05_ray_tracing ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY} glm)

include_directories(06_shaders)
add_executable(06_shaders 06_shaders/main_students.cpp 06_shaders/common.cpp 06_shaders/HUD_Logger.cpp)
target_link_libraries(06_shaders ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY} glm)