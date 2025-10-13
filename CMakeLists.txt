cmake_minimum_required(VERSION 3.10)
project(DataOrientedDesignInGameDev)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

add_executable(sdl_test main.cpp)
target_link_libraries(sdl_test ${SDL2_LIBRARIES})
