cmake_minimum_required(VERSION 3.10)
project(DataOrientedDesignInGameDev)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(find_or_build_sdl3_package PACKAGE_NAME SUBMODULE_PATH)
    # Try to find from system/vcpkg
    find_package(${PACKAGE_NAME} QUIET CONFIG)
    if (NOT ${PACKAGE_NAME}_FOUND)
        find_package(${PACKAGE_NAME} QUIET)
    endif()

    # If not found, try submodules
    if (NOT ${PACKAGE_NAME}_FOUND)
        if (EXISTS ${CMAKE_SOURCE_DIR}/${SUBMODULE_PATH}/CMakeLists.txt)
            message(STATUS "${PACKAGE_NAME} not found in system, building from submodule")

            # Configure submodule options
            if (${PACKAGE_NAME} STREQUAL "SDL3")
                set(SDL_TEST OFF CACHE BOOL "Disable SDL tests" FORCE)
                set(SDL_SHARED ON CACHE BOOL "Build shared SDL library" FORCE)
                set(SDL_STATIC OFF CACHE BOOL "Build static SDL library" FORCE)
                set(SDL_X11_XTEST OFF CACHE BOOL "Disable X11 XTEST support" FORCE)
            elseif (${PACKAGE_NAME} STREQUAL "SDL3_image")
                set(SDLIMAGE_AVIF OFF CACHE BOOL "Disable AVIF support" FORCE)
                set(SDLIMAGE_JXL OFF CACHE BOOL "Disable JPEG-XL support" FORCE)
                set(SDLIMAGE_TESTS OFF CACHE BOOL "Disable SDL_image tests" FORCE)
            elseif (${PACKAGE_NAME} STREQUAL "SDL3_ttf")
                set(SDLTTF_VENDORED OFF CACHE BOOL "Use system freetype/harfbuzz" FORCE)
                set(SDLTTF_PLUTOSVG OFF CACHE BOOL "Disable plutosvg" FORCE)
                set(SDLTTF_SAMPLES OFF CACHE BOOL "Disable SDL3_ttf samples" FORCE)
            endif()

            add_subdirectory(${SUBMODULE_PATH})
        else()
            message(FATAL_ERROR
                "${PACKAGE_NAME} not found!\n"
                "Options:\n"
                "  1. Install via package manager:\n"
                "     Linux: sudo apt install lib${PACKAGE_NAME}-dev\n"
                "     Windows: vcpkg install ${PACKAGE_NAME}:x64-windows\n"
                "  2. Initialize submodules: git submodule update --init --recursive\n")
        endif()
    else()
        message(STATUS "Using system ${PACKAGE_NAME}")
    endif()
endmacro()

# Find or build SDL3 packages
find_or_build_sdl3_package(SDL3 external/SDL)
find_or_build_sdl3_package(SDL3_image external/SDL_image)
find_or_build_sdl3_package(SDL3_ttf external/SDL_ttf)

# Build executable
add_executable(DataOrientedDesignInGameDev
        src/main.cpp
        lib/PerformanceMonitor.cpp
        lib/PerformanceMonitor.h
        lib/Particles.cpp
        lib/Particles.h
        lib/Engine.cpp
        lib/Engine.h
        lib/ECS.h
        lib/Entity.h
        lib/Utils.h
)

# Link SDL3 libraries (handles all target name variations)
foreach(LIB SDL3 SDL3_image SDL3_ttf)
    if (TARGET ${LIB}::${LIB}-shared)
        target_link_libraries(DataOrientedDesignInGameDev PRIVATE ${LIB}::${LIB}-shared)
    elseif (TARGET ${LIB}::${LIB})
        target_link_libraries(DataOrientedDesignInGameDev PRIVATE ${LIB}::${LIB})
    elseif (TARGET ${LIB}-shared)
        target_link_libraries(DataOrientedDesignInGameDev PRIVATE ${LIB}-shared)
    elseif (TARGET ${LIB})
        target_link_libraries(DataOrientedDesignInGameDev PRIVATE ${LIB})
    endif()
endforeach()

# Windows-specific libraries
if (WIN32)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE psapi)
endif()