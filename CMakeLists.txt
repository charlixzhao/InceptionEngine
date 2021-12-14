cmake_minimum_required(VERSION 3.18)

project(InceptionEngine LANGUAGES CXX VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(Source)

#set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "debug")
#set(CMAKE_BUILD_TYPE "Release" CACHE STRING "release")

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "build mode")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release") 