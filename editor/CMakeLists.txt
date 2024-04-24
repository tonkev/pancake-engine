cmake_minimum_required(VERSION 3.00)

project(pancake_editor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(pancake_editor OBJECT
  src/editor_session.cpp
)

target_include_directories(pancake_editor PUBLIC
    include/pancake_editor
    ../pancake/include
)

target_link_libraries(pancake_editor
    pancake
)