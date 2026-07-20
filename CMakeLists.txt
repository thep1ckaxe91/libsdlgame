cmake_minimum_required(VERSION 3.16)
project(libsdlgame CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB ENGINE_SOURCES "src/*.cpp")

add_library(GameEngine STATIC ${ENGINE_SOURCES})

# public here mean any unit which link this lib will able to access the header in /include
target_include_directories(GameEngine PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Fetch cpptrace
include(FetchContent)
FetchContent_Declare(
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG        v0.5.2
)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(cpptrace)

# Find SDL2 and its extensions
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

target_link_libraries(GameEngine PRIVATE 
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    cpptrace::cpptrace
)

# this was generated in an attempt for compiler optimization for SIMD
target_compile_options(GameEngine PRIVATE
    -Wall 
    -Wextra 
    -Wconversion 
    -Wshadow 
    -Wpedantic
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:
        -O3
        -ftree-vectorize 
        -march=native 
    >
    
    # Optional: Apply basic optimizations during Debug to keep the engine playable, 
    # but keep debug symbols fully intact. 
    $<$<CONFIG:Debug>:
        -O0 
        -g3
    >
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(BUILD_TESTING "Build tests" ON)
    if(BUILD_TESTING)
        enable_testing()
        add_subdirectory(tests)
    endif()
endif()
