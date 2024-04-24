cmake_minimum_required(VERSION 3.00)

project(gl3w)

add_library(gl3w
    src/gl3w.c
)

target_include_directories(gl3w PUBLIC
    include
)