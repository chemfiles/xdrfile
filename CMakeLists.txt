cmake_minimum_required(VERSION 2.8.12)

project(xdrfile C)

set(SOURCES
    src/xdrfile.c
    src/xdrfile_trr.c
    src/xdrfile_xtc.c
    include/xdrfile.h
    include/xdrfile_trr.h
    include/xdrfile_xtc.h
)

add_library(xdrfile OBJECT ${SOURCES})
target_include_directories(xdrfile PUBLIC include)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    enable_testing()

    add_executable(xdrfile_test src/xdrfile_c_test.c $<TARGET_OBJECTS:xdrfile>)
    target_include_directories(xdrfile_test PRIVATE include)
    if (UNIX)
        target_link_libraries(xdrfile_test m)
    endif()

    add_test(xdrfile xdrfile_test)
endif()
