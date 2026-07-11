option(SELS_IDE_ENABLE_CLANG_TIDY "Add a 'tidy' target that runs clang-tidy over the sources" OFF)

if(SELS_IDE_ENABLE_CLANG_TIDY)
    find_program(SELS_IDE_CLANG_TIDY_EXE
        NAMES clang-tidy
        HINTS "$ENV{ProgramFiles}/LLVM/bin"
    )
    if(NOT SELS_IDE_CLANG_TIDY_EXE)
        message(FATAL_ERROR "SELS_IDE_ENABLE_CLANG_TIDY is ON but clang-tidy was not found")
    endif()
endif()

# clang-tidy's own front end parses the sources directly, independently of
# whatever compiler actually builds the target. Wiring it in via the
# CXX_CLANG_TIDY target property makes clang-tidy part of the normal compile
# step, so a clang/MSVC front-end divergence (e.g. an overload MSVC accepts
# but Clang's stricter conformance rejects) would fail ordinary builds for
# reasons unrelated to lint findings. A standalone target keeps clang-tidy
# runs opt-in and unable to break `cmake --build`.
function(sels_ide_add_clang_tidy_target target)
    if(NOT SELS_IDE_ENABLE_CLANG_TIDY)
        return()
    endif()
    get_target_property(sources ${target} SOURCES)
    add_custom_target(tidy
        COMMAND ${SELS_IDE_CLANG_TIDY_EXE} -p "${CMAKE_BINARY_DIR}" ${sources}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy on ${target}"
        VERBATIM
    )
endfunction()
