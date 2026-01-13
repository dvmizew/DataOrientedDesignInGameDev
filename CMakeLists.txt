cmake_minimum_required(VERSION 3.16)
project(DataOrientedDesignInGameDev)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

option(DOD_FETCH_SDL "Automatically fetch SDL3 libs if not found in system" ON)

set(DOD_SDL3_TAG "release-3.4.0" CACHE STRING "SDL3 Git tag/commit to fetch when not found")
set(DOD_SDL3_IMAGE_TAG "release-3.2.6" CACHE STRING "SDL3_image Git tag/commit to fetch when not found")
set(DOD_SDL3_TTF_TAG "release-3.2.2" CACHE STRING "SDL3_ttf Git tag/commit to fetch when not found")
# Force bump if cache kept an invalid/older tag
if (DOD_SDL3_TTF_TAG VERSION_LESS "3.2.2")
    set(DOD_SDL3_TTF_TAG "release-3.2.2" CACHE STRING "SDL3_ttf Git tag/commit to fetch when not found" FORCE)
endif()

# Helper to locate or fetch SDL component
function(dod_find_or_fetch lib var_tag)
    set(_lib ${lib})
    set(_tag ${${var_tag}})

    # First try system / package manager / vcpkg
    find_package(${_lib} QUIET CONFIG)
    if (NOT ${_lib}_FOUND)
        find_package(${_lib} QUIET)
    endif()

    if (NOT ${_lib}_FOUND)
        if (NOT DOD_FETCH_SDL)
            message(FATAL_ERROR "${_lib} not found. Install it or enable DOD_FETCH_SDL to fetch automatically.")
        endif()
        message(STATUS "${_lib} not found; fetching from upstream")
        if (_lib STREQUAL "SDL3")
            set(_repo "https://github.com/libsdl-org/SDL")
            set(_opts "SDL_TEST" OFF "SDL_SHARED" ON "SDL_STATIC" OFF "SDL_X11_XTEST" OFF)
        elseif (_lib STREQUAL "SDL3_image")
            set(_repo "https://github.com/libsdl-org/SDL_image")
            set(_opts "SDLIMAGE_AVIF" OFF "SDLIMAGE_JXL" OFF "SDLIMAGE_TESTS" OFF "SDLIMAGE_VENDORED" ON)
        elseif (_lib STREQUAL "SDL3_ttf")
            set(_repo "https://github.com/libsdl-org/SDL_ttf")
            set(_opts "SDLTTF_VENDORED" ON "SDLTTF_PLUTOSVG" OFF "SDLTTF_SAMPLES" OFF)
        else()
            message(FATAL_ERROR "Unsupported SDL component: ${_lib}")
        endif()

        FetchContent_Declare(${_lib}
            GIT_REPOSITORY ${_repo}
            GIT_TAG ${_tag}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
        )
        # Apply component-specific options before configure
        list(LENGTH _opts _len)
        if (_len GREATER 1)
            math(EXPR _pairs "${_len} / 2")
            math(EXPR _last "${_pairs} - 1")
            foreach(i RANGE 0 ${_last})
                math(EXPR idx1 "${i} * 2")
                math(EXPR idx2 "${idx1} + 1")
                list(GET _opts ${idx1} key)
                list(GET _opts ${idx2} val)
                set(${key} ${val} CACHE BOOL "Auto-set for ${_lib}" FORCE)
            endforeach()
        endif()
        FetchContent_MakeAvailable(${_lib})
    else()
        message(STATUS "Using system ${_lib}")
    endif()
endfunction()

# Locate/fetch SDL components
dod_find_or_fetch(SDL3 DOD_SDL3_TAG)
dod_find_or_fetch(SDL3_image DOD_SDL3_IMAGE_TAG)
dod_find_or_fetch(SDL3_ttf DOD_SDL3_TTF_TAG)

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

# Link SDL3 libraries (handles target name variations)
foreach(LIB SDL3 SDL3_image SDL3_ttf)
    if (TARGET ${LIB}::${LIB}-shared)
        target_link_libraries(DataOrientedDesignInGameDev PRIVATE ${LIB}::${LIB}-shared)
    elseif (TARGET ${LIB}::${LIB})
        target_link_libraries(DataOrientedDesignInGameDev PRIVATE ${LIB}::${LIB})
    elseif (TARGET ${LIB}-shared)
        target_link_libraries(DataOrientedDesignInGameDev PRIVATE ${LIB}-shared)
    elseif (TARGET ${LIB})
        target_link_libraries(DataOrientedDesignInGameDev PRIVATE ${LIB})
    else()
        message(FATAL_ERROR "Expected target for ${LIB} not found; ensure package provides CMake config.")
    endif()
endforeach()

# Windows-specific libraries
if (WIN32)
    target_link_libraries(DataOrientedDesignInGameDev PRIVATE psapi)
endif()