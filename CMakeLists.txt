cmake_minimum_required(VERSION 3.31)
project(untitled1)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled1 src/main.cpp
        src/stb_image_write.h
        src/lodepng.cpp
)
