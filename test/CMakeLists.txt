macro(make_test target source library)
    add_executable(${target} ${source})
    target_link_libraries(${target} tap ${library})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endmacro(make_test)

make_test(filesystem filesystem.c kioku)
add_definitions(-DBUILDDIR="${CMAKE_CURRENT_BINARY_DIR}")
