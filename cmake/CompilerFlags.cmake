# Standard warning flags per supported compiler toolchain.
# AppleClang/Clang for macOS, MSVC (cl.exe) for Windows.

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    set(SELS_IDE_COMPILE_OPTIONS
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(SELS_IDE_COMPILE_OPTIONS
        /W4
        /permissive-
        /Zc:__cplusplus
        /EHsc
        /utf-8
    )
endif()
