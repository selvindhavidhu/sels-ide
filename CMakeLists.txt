cmake_minimum_required(VERSION 4.0.0)

project(sels-ide
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt CONFIG REQUIRED)
find_package(tvision CONFIG REQUIRED)

include(cmake/CompilerFlags.cmake)
include(cmake/ClangTidy.cmake)

add_executable(sels-ide
    src/main.cpp
)

target_compile_features(sels-ide PRIVATE cxx_std_20)

target_compile_options(sels-ide PRIVATE ${SELS_IDE_COMPILE_OPTIONS})

target_link_libraries(sels-ide PRIVATE fmt::fmt tvision::tvision)

sels_ide_add_clang_tidy_target(sels-ide)
