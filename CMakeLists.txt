cmake_minimum_required(VERSION 3.16) 

project(my-raja-application) 

set(CMAKE_CXX_FLAGS "-Wall ")

find_package(RAJA REQUIRED) 

add_executable(testraja.exe testraja.cpp)

target_link_libraries(testraja.exe RAJA) 
