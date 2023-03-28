cmake_minimum_required(VERSION 3.21)
project(openGL)

file(GLOB SRC_FILES
        ${PROJECT_SOURCE_DIR}/*.cpp
        ${PROJECT_SOURCE_DIR}/include/*.h
        ${PROJECT_SOURCE_DIR}/include/*.hpp
        ${PROJECT_SOURCE_DIR}/include/GL/*.h
        ${PROJECT_SOURCE_DIR}/src/*.cpp)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
link_directories(freeglut/lib/x64)
add_executable(openGL ${SRC_FILES} GLSL.h GLCanvas.cpp GLCanvas.h)

target_link_libraries(openGL libfreeglut.a opengl32 libglu32.a)