cmake_minimum_required(VERSION 3.14)

project(dec64
    VERSION 0.1.0
    DESCRIPTION "A fast and lightweight decimal64 implementation"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dec64
    src/dec64.cpp
    include/dec64.hpp
)

target_include_directories(dec64 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(dec64 PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -O3>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

if(MINGW)
    target_link_options(dec64 PRIVATE -static-libgcc -static-libstdc++)
endif()

option(BUILD_EXAMPLES "Build examples" ON)
#option(BUILD_TESTS    "Build tests"    OFF)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

#if(BUILD_TESTS)
    #enable_testing()
    #add_subdirectory(tests)
#endif()