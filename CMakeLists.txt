cmake_minimum_required(VERSION 3.10)
project(DataOrientedDesignInGameDev)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# System packages first, if not then clone SDL from repo
find_package(SDL3 QUIET)
if (NOT SDL3_FOUND)
    message(STATUS "SDL3 not found in system, using vendored submodule")
    if (EXISTS ${CMAKE_SOURCE_DIR}/external/SDL3/CMakeLists.txt)
        set(SDL_TEST OFF CACHE BOOL "Disable SDL tests" FORCE)
        set(SDL_SHARED ON CACHE BOOL "Build shared SDL library" FORCE)
        set(SDL_STATIC OFF CACHE BOOL "Build static SDL library" FORCE)
        set(SDL_X11_XTEST OFF CACHE BOOL "Disable X11 XTEST support" FORCE)
        add_subdirectory(external/SDL3)
    else()
        message(FATAL_ERROR "SDL3 not found in system and submodule not initialized. Run: git submodule update --init --recursive")
    endif()
else()
    message(STATUS "Using system SDL3")
endif()

find_package(SDL3_image QUIET)
if (NOT SDL3_image_FOUND)
    message(STATUS "SDL3_image not found in system, using vendored submodule")
    if (EXISTS ${CMAKE_SOURCE_DIR}/external/SDL3_image/CMakeLists.txt)
        set(SDLIMAGE_AVIF OFF CACHE BOOL "Disable AVIF support" FORCE)
        set(SDLIMAGE_JXL OFF CACHE BOOL "Disable JPEG-XL support" FORCE)
        set(SDLIMAGE_TESTS OFF CACHE BOOL "Disable SDL_image tests" FORCE)
        add_subdirectory(external/SDL3_image)
    else()
        message(FATAL_ERROR "SDL3_image not found in system and submodule not initialized. Run: git submodule update --init --recursive")
    endif()
else()
    message(STATUS "Using system SDL3_image")
endif()

find_package(SDL3_ttf QUIET)
if (NOT SDL3_ttf_FOUND)
    message(STATUS "SDL3_ttf not found in system, using vendored submodule")
    if (EXISTS ${CMAKE_SOURCE_DIR}/external/SDL3_ttf/CMakeLists.txt)
        set(SDLTTF_VENDORED ON CACHE BOOL "Use vendored freetype/harfbuzz" FORCE)
        set(SDLTTF_PLUTOSVG OFF CACHE BOOL "Disable plutosvg" FORCE)
        set(SDLTTF_SAMPLES OFF CACHE BOOL "Disable SDL3_ttf samples" FORCE)
        add_subdirectory(external/SDL3_ttf)
    else()
        message(FATAL_ERROR "SDL3_ttf not found in system and submodule not initialized. Run: git submodule update --init --recursive")
    endif()
else()
    message(STATUS "Using system SDL3_ttf")
endif()

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

if (TARGET SDL3::SDL3-shared)
    target_link_libraries(imgui PUBLIC SDL3::SDL3-shared)
elseif (TARGET SDL3::SDL3)
    target_link_libraries(imgui PUBLIC SDL3::SDL3)
else()
    # Fallback for vendored SDL3 without namespace
    target_link_libraries(imgui PUBLIC SDL3-shared)
    target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/external/SDL3/include)
endif()

add_executable(DataOrientedDesignInGameDev
        src/main.cpp
        lib/PerformanceMonitor.cpp
        lib/PerformanceMonitor.h
        lib/SpriteOOP.cpp
        lib/SpriteOOP.h
        lib/SpriteDOD.cpp
        lib/SpriteDOD.h
)

# Link libraries (supports both system and vendored SDL)
target_link_libraries(DataOrientedDesignInGameDev PRIVATE imgui)

if (TARGET SDL3::SDL3-shared)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE SDL3::SDL3-shared)
elseif (TARGET SDL3::SDL3)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE SDL3::SDL3)
else()
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE SDL3-shared)
endif()

if (TARGET SDL3_image::SDL3_image-shared)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE SDL3_image::SDL3_image-shared)
elseif (TARGET SDL3_image::SDL3_image)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE SDL3_image::SDL3_image)
else()
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE SDL3_image-shared)
endif()

if (TARGET SDL3_ttf::SDL3_ttf-shared)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE SDL3_ttf::SDL3_ttf-shared)
elseif (TARGET SDL3_ttf::SDL3_ttf)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE SDL3_ttf::SDL3_ttf)
else()
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE SDL3_ttf-shared)
endif()

if (WIN32)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE psapi)
endif()