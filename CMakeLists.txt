cmake_minimum_required(VERSION 3.5)
project(PlayDirect_App CXX)

set(CMAKE_CXX_COMPLIER "C:/msys64/ucrt64/bin/g++.exe")
set(CMAKE_C_COMPLIER "C:/msys64/ucrt64/bin/gcc.exe")
set(SDL2_DIR "./ext/sdl")

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(EXT_DIR ${CMAKE_SOURCE_DIR}/ext)

## Libraries
find_package(SDL2 REQUIRED)
add_subdirectory(PlayDirect)