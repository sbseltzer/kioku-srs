cmake_minimum_required (VERSION 2.6)

project (Kioku)
include_directories( ${CMAKE_SOURCE_DIR}/include
                     ${CMAKE_SOURCE_DIR}/extern/mongoose
                     ${CMAKE_SOURCE_DIR}/extern/parson
                     ${CMAKE_SOURCE_DIR}/extern/libgit2/include
                     ${CMAKE_SOURCE_DIR}/extern/utf8.h
                     ${CMAKE_SOURCE_DIR}/extern/generic-c-hashmap
                     ) 

# find_package(LibGit2 REQUIRED)
# find_package(OpenSSL REQUIRED)
# find_package(LibCURL REQUIRED)

add_executable( kioku
                src/main.c
                src/model.c
                src/controller.c
                src/rest.c
                src/server.c
                extern/parson/parson.c
                extern/mongoose/mongoose.c
                )
