cmake_minimum_required(VERSION 3.12)

project(stb_image)

add_library(stb_image STATIC include/stb_image.h src/stb_image.cpp)

target_include_directories(stb_image PUBLIC include)