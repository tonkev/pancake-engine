cmake_minimum_required(VERSION 3.00)

project(external)

add_subdirectory(gl3w)
add_subdirectory(SDL)

add_library(imgui
  imgui/backends/imgui_impl_opengl3.cpp
  imgui/backends/imgui_impl_sdl3.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui.cpp
)

target_include_directories(imgui PUBLIC
  SDL/include
  imgui
)

target_link_libraries(imgui
  SDL3-shared
)