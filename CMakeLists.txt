cmake_minimum_required (VERSION 2.6)

project (Kioku)
include_directories( ${CMAKE_SOURCE_DIR}/include
                     ${CMAKE_SOURCE_DIR}/extern/mongoose
                     ${CMAKE_SOURCE_DIR}/extern/parson
                     ${CMAKE_SOURCE_DIR}/extern/libgit2/include
                     ${CMAKE_SOURCE_DIR}/extern/utf8.h
                     ${CMAKE_SOURCE_DIR}/extern/lua
                     ${CMAKE_SOURCE_DIR}/extern/generic-c-hashmap
                     ) 

# find_package(LibGit2 REQUIRED)
# find_package(OpenSSL REQUIRED)
# find_package(LibCURL REQUIRED)

add_library(lua5-3-4 OBJECT ${CMAKE_SOURCE_DIR}/extern/lua/ldblib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lgc.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/llex.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lmem.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lparser.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lstate.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/ltable.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lzio.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lapi.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lbaselib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lcorolib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/ldebug.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/ldump.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lopcodes.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lstring.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/ltm.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lutf8lib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lbitlib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lctype.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lfunc.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lfunc.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/linit.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/loadlib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/ltablib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lvm.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lauxlib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lcode.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/ldo.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/liolib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lmathlib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lobject.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/loslib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lstrlib.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/ltests.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lua.c
                            ${CMAKE_SOURCE_DIR}/extern/lua/lundump.c
                            )
add_executable( kioku
                src/main.c
                src/model.c
                src/controller.c
                src/rest.c
                src/server.c
                extern/parson/parson.c
                extern/mongoose/mongoose.c
                )
add_dependencies( kioku lua5-3-4 )
