add_executable( kioku-server
                main.c
                ${CMAKE_SOURCE_DIR}/extern/mongoose/mongoose.c
                ${CMAKE_SOURCE_DIR}/extern/parson/parson.c
)
target_link_libraries( kioku-server kioku ${KIOKU_LIBS})
add_dependencies( kioku-server kioku )
