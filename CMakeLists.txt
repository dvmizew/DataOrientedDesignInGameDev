cmake_minimum_required(VERSION 3.10)
project(DataOrientedDesignInGameDev)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable(DataOrientedDesignInGameDev
        src/main.cpp
)

target_link_libraries(DataOrientedDesignInGameDev
        PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
)