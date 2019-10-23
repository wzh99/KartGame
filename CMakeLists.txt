cmake_minimum_required(VERSION 3.0)
project(lab02)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DGLEW_STATIC)
include_directories(include)

aux_source_directory(src DIR_SRC)

link_directories(lib)
link_libraries(assimp z glew32s glfw3 opengl32)

add_executable(${PROJECT_NAME} ${DIR_SRC})
