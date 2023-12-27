cmake_minimum_required(VERSION 3.5)
project(PlayDirect_App CXX)

set(CMAKE_CXX_COMPLIER "C:/msys64/ucrt64/bin/g++.exe")
set(CMAKE_C_COMPLIER "C:/msys64/ucrt64/bin/gcc.exe")
set(SDL2_DIR "./ext/sdl")


## Libraries
#add_subdirectory( ext/sdl )
find_package(SDL2 REQUIRED)

add_executable(PlayDirect ./src/main.cpp)

## Link Libraries
target_include_directories(PlayDirect PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(PlayDirect PRIVATE ${SDL2_LIBRARIES})