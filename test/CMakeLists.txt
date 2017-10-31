
macro(make_test target source)
    add_executable(${target} ${source})
    add_dependencies(${target} kioku)
    target_link_libraries(${target} kioku ${KIOKU_LIBS})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endmacro(make_test)

make_test(filesystem filesystem.c)
make_test(git git.c)
make_test(model model.c)
make_test(schedule schedule.c)

add_definitions(-DBUILDDIR="${CMAKE_CURRENT_BINARY_DIR}")
