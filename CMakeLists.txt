cmake_minimum_required(VERSION 3.6)
project(billiards)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(billiards ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(billiards ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )