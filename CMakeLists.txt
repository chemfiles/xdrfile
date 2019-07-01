cmake_minimum_required(VERSION 2.8.12)

project(xdrfile C)

if (POLICY CMP0063)
    # Use of `<LANG>_VISIBILITY_PRESET` in OBJECT libraries
    cmake_policy(SET CMP0063 NEW)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCES
    src/xdrfile.c
    src/xdrfile_trr.c
    src/xdrfile_xtc.c
    include/xdrfile.h
    include/xdrfile_trr.h
    include/xdrfile_xtc.h
)

add_library(libxdrfile OBJECT ${SOURCES})
target_include_directories(libxdrfile PUBLIC include)

if (${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
    enable_testing()
    add_subdirectory(tests)
endif()
