cmake_minimum_required(VERSION 3.18)

project(logifix-demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC -fno-gnu-unique")

set(LOGIFIX_BUILD_LIBRARY ON)

add_subdirectory(logifix)
add_subdirectory(imgui-boilerplate)

include_directories(imgui-boilerplate/imgui/)
include_directories(logifix/src)
include_directories(logifix/vendor/souffle/src/include)
include_directories(logifix/vendor/sjp)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(program SHARED program.cpp)

target_link_libraries(program logifix)

add_executable(main main.cpp editor.cpp ast.cpp filepicker.cpp imedit/imedit.cpp ImGuiFileDialog/ImGuiFileDialog.cpp)
target_link_libraries(main imgui-boilerplate SDL2 dl pthread GL logifix)
