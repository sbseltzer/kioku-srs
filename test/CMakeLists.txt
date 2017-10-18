
macro(make_test target source library)
    add_executable(${target} ${source} ${CMAKE_SOURCE_DIR}/extern/libtap/tap.c)
    add_dependencies(${target} kioku)
    target_link_libraries(${target} git2 ${library})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endmacro(make_test)

make_test(filesystem filesystem.c kioku)
make_test(git git.c kioku)

add_definitions(-DBUILDDIR="${CMAKE_CURRENT_BINARY_DIR}")
