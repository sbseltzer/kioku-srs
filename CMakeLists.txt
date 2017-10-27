cmake_minimum_required (VERSION 2.6)

project (kioku C)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Give this to get CMake to look for libraries built from source.
list(APPEND CMAKE_PREFIX_PATH
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/build
            ${CMAKE_SOURCE_DIR}/extern/libgit2
            ${CMAKE_SOURCE_DIR}/extern/libgit2/build
            ${CMAKE_SOURCE_DIR}/extern/libgit2/src
            ${CMAKE_SOURCE_DIR}/extern/libssh2
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src
            ${CMAKE_SOURCE_DIR}/extern/libssh2/build
            ${CMAKE_SOURCE_DIR}/extern/libssh2/build/src
            )

# These will only help builds look for the libraries, but DOES NOT necessarily make builds use that configuration.
# For Visual Studio builds, one must also set the configuration at build time, and must have done it for all dependencies.
# https://stackoverflow.com/questions/19024259/how-to-change-the-build-type-to-release-mode-in-cmake#20423820
# TODO: Figure out a convenient strategy for disambiguating debug/release builds. I'm thinking do what Orx does and add a 'd' to the library name for debug.
list(APPEND CMAKE_PREFIX_PATH
            ${CMAKE_SOURCE_DIR}/Debug
            ${CMAKE_SOURCE_DIR}/build/Debug
            ${CMAKE_SOURCE_DIR}/extern/libgit2/Debug
            ${CMAKE_SOURCE_DIR}/extern/libgit2/build/Debug
            ${CMAKE_SOURCE_DIR}/extern/libgit2/src/Debug
            ${CMAKE_SOURCE_DIR}/extern/libssh2/Debug
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src/Debug
            ${CMAKE_SOURCE_DIR}/extern/libssh2/build/Debug
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src/build/Debug
            )
list(APPEND CMAKE_PREFIX_PATH
            ${cmake_source_dir}/Release
            ${CMAKE_SOURCE_DIR}/build/Release
            ${CMAKE_SOURCE_DIR}/extern/libgit2/Release
            ${CMAKE_SOURCE_DIR}/extern/libgit2/build/Release
            ${CMAKE_SOURCE_DIR}/extern/libgit2/src/Release
            ${CMAKE_SOURCE_DIR}/extern/libssh2/Release
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src/Release
            ${CMAKE_SOURCE_DIR}/extern/libssh2/build/Release
            ${CMAKE_SOURCE_DIR}/extern/libssh2/src/build/Release
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
#find_package(LibGit2 REQUIRED)
find_package(Pthreads)
find_package(OpenSSL)
find_package(LibCURL)

# We build libssh2 from source, so simply take care of some Windows naming weirdness.
if (WIN32)
  set(LIBSSH_LIB libssh2)
else()
  set(LIBSSH_LIB ssh2)
endif (WIN32)

set (KIOKU_LIBS git2 http-parser ${LIBSSH_LIB})

if (PTHREADS_FOUND)
   include_directories( ${Pthreads_INCLUDE_DIRS} )
   list( APPEND KIOKU_LIBS ${Pthreads_LIBRARIES} )
endif()
if (LIBCURL_FOUND)
   include_directories( ${LIBCURL_INCLUDE_DIR} )
   list( APPEND KIOKU_LIBS ${LIBCURL_LIBRARY} )
endif()
if (OPENSSL_FOUND)
   include_directories( ${OPENSSL_INCLUDE_DIR} )
   list( APPEND KIOKU_LIBS ${OPENSSL_LIBRARIES} )
endif()

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

link_directories(
  ${CMAKE_SOURCE_DIR}/extern/libgit2
  ${CMAKE_SOURCE_DIR}/extern/libgit2/Debug
  ${CMAKE_SOURCE_DIR}/extern/libgit2/Release
  ${CMAKE_SOURCE_DIR}/extern/libgit2/build
  ${CMAKE_SOURCE_DIR}/extern/libgit2/build/Debug
  ${CMAKE_SOURCE_DIR}/extern/libgit2/build/Release
  ${CMAKE_SOURCE_DIR}/extern/libgit2/build/deps/http-parser
  ${CMAKE_SOURCE_DIR}/extern/libssh2
  ${CMAKE_SOURCE_DIR}/extern/libssh2/Debug
  ${CMAKE_SOURCE_DIR}/extern/libssh2/Release
  ${CMAKE_SOURCE_DIR}/extern/libssh2/build
  ${CMAKE_SOURCE_DIR}/extern/libssh2/build/Debug
  ${CMAKE_SOURCE_DIR}/extern/libssh2/build/Release
  ${CMAKE_SOURCE_DIR}/extern/libssh2/build/src
  ${CMAKE_SOURCE_DIR}/extern/libssh2/build/src/Debug
  ${CMAKE_SOURCE_DIR}/extern/libssh2/build/src/Release
)

include_directories( include
                     extern/mongoose
                     extern/parson
                     extern/libgit2/include
                     extern/utf8.h
                     extern/lua
                     extern/generic-c-hashmap
                     extern/tinydir
                     extern/libtap
                     extern/greatest
                     extern/libssh2/src/
                     extern/libssh2/include
                     extern/libgit2/deps/http_parser/
                     ) 

add_library( kioku SHARED src/kioku/log.c
                          src/kioku/filesystem.c
                          src/kioku/simplegit.c
                          src/kioku/model.c
                          src/kioku/controller.c
                          src/kioku/rest.c
                          src/kioku/server.c
                          extern/parson/parson.c
                          )

if( MSVC )
# For now, we will always link statically to the CRT
  set(STATIC_CRT ON)
# Shamelessly copied from libgit2
	IF (STATIC_CRT)
		SET(CRT_FLAG_DEBUG "/MTd")
		SET(CRT_FLAG_RELEASE "/MT")
	ELSE()
		SET(CRT_FLAG_DEBUG "/MDd")
		SET(CRT_FLAG_RELEASE "/MD")
	ENDIF()
endif ()
target_link_libraries( kioku ${KIOKU_LIBS} )
# Edited from https://github.com/commontk/Log4Qt/blob/patched/CMakeLists.txt#L116
if (NOT MSVC)
   include(CheckCCompilerFlag)
   CHECK_C_COMPILER_FLAG("-fPIC" HAS_FPIC)
   IF( HAS_FPIC AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
       SET_TARGET_PROPERTIES(kioku PROPERTIES COMPILE_FLAGS "-fPIC")
   ENDIF()
endif ()

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

#set( CTEST_MEMORYCHECK_COMMAND valgrind )
#set( CTEST_MEMORYCHECK_COMMAND_OPTIONS "--tool=callgrind -v" )
add_test(NAME TestFileSystem COMMAND filesystem)
add_test(NAME TestGit COMMAND git)
add_test(NAME TestModel COMMAND model)
