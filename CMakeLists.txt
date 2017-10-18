cmake_minimum_required (VERSION 2.6)

project (kioku)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Give this to get CMake to look for libraries built from source.
list(APPEND CMAKE_PREFIX_PATH
            ${CMAKE_SOURCE_DIR}/extern/libgit2
            ${CMAKE_SOURCE_DIR}/extern/libgit2/Release
            ${CMAKE_SOURCE_DIR}/extern/libgit2/Debug
            ${CMAKE_SOURCE_DIR}/extern/libgit2/build
            ${CMAKE_SOURCE_DIR}/extern/libgit2/build/Release
            ${CMAKE_SOURCE_DIR}/extern/libgit2/build/Debug
            ${CMAKE_SOURCE_DIR}/extern/libgit2/src
            ${CMAKE_SOURCE_DIR}/extern/libgit2/src/Release
            ${CMAKE_SOURCE_DIR}/extern/libgit2/src/Debug
            ${CMAKE_SOURCE_DIR}/extern/libssh2
            ${CMAKE_SOURCE_DIR}/extern/libssh2/Release
            ${CMAKE_SOURCE_DIR}/extern/libssh2/Debug
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src/Release
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src/Debug
            ${CMAKE_SOURCE_DIR}/extern/libssh2/build
            ${CMAKE_SOURCE_DIR}/extern/libssh2/build/Release
            ${CMAKE_SOURCE_DIR}/extern/libssh2/build/Debug
            ${CMAKE_SOURCE_DIR}/extern/libssh2/build/src
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src/build/Release
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src/build/Debug
            )

# Give this to find Libssh2Config.cmake
#set(Libssh2_DIR extern/libssh2/src)
#set(Libssh2_DIR extern/libssh2/build/src)

##
# Some platforms won't have the specific required version of libraries.
# In those cases, a downloaded pre-built one will be used for compilation/packaging.
# These could be served via Git-LFS from a subrepository.
# Take a look at http://www.mantidproject.org/Building_with_CMake
# Also look at how the Orx Project automates setup.
##

#find_package(ZLIB)
#find_package(LibSSH2)
#find_package(Libssh2 1.8.1 REQUIRED)
#find_package(OpenSSL REQUIRED)
#find_package(LibCURL REQUIRED)
#find_package(LibGit2 REQUIRED)


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

link_directories( ${CMAKE_SOURCE_DIR}/extern/libssh2/src/Debug
                  ${CMAKE_SOURCE_DIR}/extern/libgit2/build/Debug
                  )
include_directories( include
                     extern/mongoose
                     extern/parson
                     extern/ims-json/src
                     extern/libgit2/include
                     extern/utf8.h
                     extern/lua
                     extern/generic-c-hashmap
                     extern/tinydir
                     extern/libtap
                     extern/greatest
                     extern/libssh2/include
                     ) 

add_library( kioku SHARED src/kioku/log.c
                          src/kioku/filesystem.c
                          src/kioku/simplegit.c
                          src/kioku/model.c
                          src/kioku/controller.c
                          src/kioku/rest.c
                          src/kioku/server.c
                          extern/parson/parson.c
                          extern/ims-json/src/json.c
                          )
target_link_libraries( kioku git2 libssh2 )
find_library( KIOKU_LIBRARY_PATH kioku ${LIBRARY_OUTPUT_DIRECTORY})
target_include_directories(kioku PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable( kioku-server
                src/main.c
                extern/mongoose/mongoose.c
                extern/parson/parson.c
                )
target_link_libraries( kioku-server kioku git2 )
add_dependencies( kioku-server kioku )

# add_subdirectory(src)
add_subdirectory(test)

enable_testing()

add_test(NAME TestFileSystem COMMAND filesystem)
add_test(NAME TestGit COMMAND git)
