cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(publisher)

# Find Gazebo
find_package(gazebo REQUIRED)
include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")

add_subdirectory(pybind11)
pybind11_add_module(publisher publisher.cpp)

target_link_libraries(publisher ${GAZEBO_LIBRARIES})
