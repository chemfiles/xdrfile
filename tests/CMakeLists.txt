function(create_test _file_)
    get_filename_component(_name_ ${_file_} NAME_WE)

    add_executable(${_name_} ${_file_} $<TARGET_OBJECTS:libxdrfile>)
    target_include_directories(${_name_} PRIVATE ${PROJECT_SOURCE_DIR}/include)

    if (UNIX)
        target_link_libraries(${_name_} m)
    endif()

    add_test(${_name_} ${_name_})
endfunction()

create_test(xdrfile.c)
create_test(xtc.c)
create_test(trr.c)
