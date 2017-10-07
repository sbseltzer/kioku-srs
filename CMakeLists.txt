cmake_minimum_required (VERSION 2.6)

project (kioku)
include_directories( include
                     extern/mongoose
                     extern/parson
                     extern/libgit2/include
                     extern/utf8.h
                     extern/lua
                     extern/generic-c-hashmap
                     extern/tinydir
                     extern/libtap
                     ) 

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
#find_package(LibGit2 REQUIRED)
#find_package(OpenSSL REQUIRED)
#find_package(LibCURL REQUIRED)

add_library(lua5-3-4 SHARED extern/lua/ldblib.c
                            extern/lua/lgc.c
                            extern/lua/llex.c
                            extern/lua/lmem.c
                            extern/lua/lparser.c
                            extern/lua/lstate.c
                            extern/lua/ltable.c
                            extern/lua/lzio.c
                            extern/lua/lapi.c
                            extern/lua/lbaselib.c
                            extern/lua/lcorolib.c
                            extern/lua/ldebug.c
                            extern/lua/ldump.c
                            extern/lua/lopcodes.c
                            extern/lua/lstring.c
                            extern/lua/ltm.c
                            extern/lua/lutf8lib.c
                            extern/lua/lbitlib.c
                            extern/lua/lctype.c
                            extern/lua/lfunc.c
                            extern/lua/lfunc.c
                            extern/lua/linit.c
                            extern/lua/loadlib.c
                            extern/lua/ltablib.c
                            extern/lua/lvm.c
                            extern/lua/lauxlib.c
                            extern/lua/lcode.c
                            extern/lua/ldo.c
                            extern/lua/liolib.c
                            extern/lua/lmathlib.c
                            extern/lua/lobject.c
                            extern/lua/loslib.c
                            extern/lua/lstrlib.c
                            extern/lua/ltests.c
                            extern/lua/lua.c
                            extern/lua/lundump.c
                            )
add_library( kioku SHARED src/kioku/log.c
                          src/kioku/filesystem.c
                          src/kioku/model.c
                          src/kioku/controller.c
                          src/kioku/rest.c
                          src/kioku/server.c
                          extern/parson/parson.c
                          )
target_link_libraries( kioku )
find_library( KIOKU_LIBRARY_PATH kioku ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(kioku PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable( kioku-server
                src/main.c
                extern/mongoose/mongoose.c
                )
target_link_libraries( kioku-server kioku )
add_dependencies( kioku-server kioku )

# add_subdirectory(src)
add_subdirectory(test)

enable_testing()

add_library(tap SHARED extern/libtap/tap.c)

add_test(NAME TestFileSystem COMMAND filesystem)
