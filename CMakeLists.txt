cmake_minimum_required(VERSION 3.10)
project(DataOrientedDesignInGameDev)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

add_library(imgui
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/backends/imgui_impl_sdl3.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
)

add_executable(DataOrientedDesignInGameDev
        src/main.cpp
        lib/PerformanceMonitor.cpp
        lib/PerformanceMonitor.h
        lib/SpriteOOP.cpp
        lib/SpriteOOP.h
        lib/SpriteDOD.cpp
        lib/SpriteDOD.h
)

target_link_libraries(DataOrientedDesignInGameDev PRIVATE
    imgui
    SDL3::SDL3
    SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf
)

if (WIN32)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE psapi)
endif()