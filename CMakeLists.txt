cmake_minimum_required (VERSION 2.6)

project (kioku C)

OPTION (BUILD_SHARED_LIBS "Build Shared Library (OFF for Static)" ON)
OPTION (BUILD_TESTS "Build Tests" ON)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/extern/libgit2/cmake/Modules/")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/extern/libssh2/cmake/")

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
# Give this to find Libssh2Config.cmake
set(Libssh2_DIR ${CMAKE_SOURCE_DIR}/extern/install/lib/cmake/libssh2)

##
# Some platforms won't have the specific required version of libraries.
# In those cases, a downloaded pre-built one could be used for compilation/packaging.
# These could be served via Git-LFS from a subrepository.
# Take a look at http://www.mantidproject.org/Building_with_CMake
# Also look at how the Orx Project automates setup.
##

INCLUDE(FindPkgConfig)

set (KIOKU_LIBS "")
find_package(Libssh2)
if (NOT Libssh2_FOUND)
  message("Failed to find libssh2 via CMake Config - try PkgConfig")
  PKG_CHECK_MODULES(Libssh2 libssh2)
  if (Libssh2_FOUND)
      message("Found libssh2 via PkgConfig")
  endif()
else()
  message("Found libssh2 via CMake Config")
endif()
if (Libssh2_FOUND)
   message("Adding libssh2")
   include_directories( ${Libssh2_INCLUDE_DIRS} ${Libssh2_INCLUDES} )
   link_directories( ${Libssh2_LIBRARY_DIRS} )
   link_libraries( ${Libssh2_LIBRARIES} ${Libssh2_LIBS} )
   list( APPEND KIOKU_LIBS ${Libssh2_LIBRARIES} ${Libssh2_LIBS}  )
else()
  # We build libssh2 from source, so simply take care of some Windows naming weirdness.
  message("Couldn't find libssh2 via CMake Config or PkgConfig - try asserting its existence based on expected build locations.")
  if (WIN32)
    set(LIBSSH_LIB libssh2)
  else()
    set(LIBSSH_LIB ssh2)
  endif (WIN32)
  link_directories( ${CMAKE_SOURCE_DIR}/extern/install/lib )
  include_directories( ${CMAKE_SOURCE_DIR}/extern/install/include )
  list( APPEND KIOKU_LIBS ${LIBSSH_LIB} )
endif()

PKG_CHECK_MODULES(LIBGIT2 libgit2)
# PkgConfig won't find it on Windows and such, so assume it was built from source and just add it
if (NOT LIBGIT2_FOUND)
   message("try libgit2 from install")
   set(LIBGIT2_FOUND YES)
   set(LIBGIT2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extern/install/include)
   set(LIBGIT2_LIBRARY_DIRS ${CMAKE_SOURCE_DIR}/extern/install/lib)
   set(LIBGIT2_LIBRARIES git2 ssh2)
endif()
if (LIBGIT2_FOUND)
   message("Adding libgit2")
   include_directories( ${LIBGIT2_INCLUDE_DIRS} )
   link_directories( ${LIBGIT2_LIBRARY_DIRS} )
   link_libraries( ${LIBGIT2_LIBRARIES} )
   list( APPEND KIOKU_LIBS ${LIBSSH_LIB} )
endif()

find_package(ZLIB)
if (ZLIB_FOUND)
   include_directories( ${ZLIB_INCLUDE_DIRS} )
   list( APPEND KIOKU_LIBS ${ZLIB_LIBRARIES} )
endif()

find_package(Pthreads)
if (Pthreads_FOUND)
   include_directories( ${Pthreads_INCLUDE_DIRS} )
   list( APPEND KIOKU_LIBS ${Pthreads_LIBRARIES} )
endif()

find_package(Threads)
if (Threads_FOUND)
   list( APPEND KIOKU_LIBS ${CMAKE_THREAD_LIBS_INIT} )
endif()

find_package(LibCURL)
PKG_CHECK_MODULES(CURL libcurl)
if (LIBCURL_FOUND)
   include_directories( ${LIBCURL_INCLUDE_DIR} )
   list( APPEND KIOKU_LIBS ${LIBCURL_LIBRARY} )
endif()

# TODO: Not really sure what the best way is to ensure we link to the right libgcrypt for our other deps to work
# See how libssh2 gets it's crypto backend in extern/libssh2/src/CMakeLists.txt
# This only seems to be necessary on macOS for some reason
find_package(Libgcrypt)
if(LIBGCRYPT_FOUND)
   list( APPEND KIOKU_LIBS ${LIBGCRYPT_LIBRARIES} )
endif()

PKG_CHECK_MODULES(OPENSSL openssl)
if (OPENSSL_FOUND)
   include_directories( ${OPENSSL_INCLUDE_DIR} )
   list( APPEND KIOKU_LIBS ${OPENSSL_LIBRARIES} )
   list( APPEND KIOKU_LIBS ${OPENSSL_SSL_LIBRARY} )
   list( APPEND KIOKU_LIBS ${OPENSSL_CRYPTO_LIBRARY} )
endif()
 
find_package(Iconv)
if (ICONV_FOUND)
   include_directories( ${ICONV_INCLUDE_DIR} )
   list( APPEND KIOKU_LIBS ${ICONV_LIBRARIES} )
endif()

# On OSX, this answer helped me: https://stackoverflow.com/questions/27662321/how-to-include-c-static-libraries-in-cmake-project-on-mac-os-x
if (APPLE)
   find_library(corefoundation_lib CoreFoundation)
   find_library(cfnetwork_lib CFNetwork)
   
   list( APPEND KIOKU_LIBS ${corefoundation_lib} )
   list( APPEND KIOKU_LIBS ${cfnetwork_lib} )
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
  ${CMAKE_SOURCE_DIR}/extern/libgit2/build
  ${CMAKE_SOURCE_DIR}/extern/libssh2
  ${CMAKE_SOURCE_DIR}/extern/libssh2/build
  ${CMAKE_SOURCE_DIR}/extern/libssh2/build/src
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

if (NOT MSVC)
   if (BUILD_SHARED_LIBS)
      set(LIBRARY_TYPE SHARED)
   else()
      set(LIBRARY_TYPE STATIC)
   endif()
endif ()
add_library( kioku ${LIBRARY_TYPE}
                   src/kioku/log.c
                   src/kioku/filesystem.c
                   src/kioku/simplegit.c
                   src/kioku/model.c
                   src/kioku/controller.c
                   src/kioku/rest.c
                   src/kioku/server.c
                   extern/parson/parson.c
                   )
target_link_libraries( kioku ${KIOKU_LIBS} )
# Edited from https://github.com/commontk/Log4Qt/blob/patched/CMakeLists.txt#L116
if (NOT MSVC)
   include(CheckCCompilerFlag)
   if (BUILD_SHARED_LIBS)
      CHECK_C_COMPILER_FLAG("-fPIC" HAS_FPIC)
      IF( HAS_FPIC AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
          SET_TARGET_PROPERTIES(kioku PROPERTIES COMPILE_FLAGS "-fPIC")
      ENDIF()
   endif()
endif ()


find_library( KIOKU_LIBRARY_PATH kioku ${LIBRARY_OUTPUT_DIRECTORY})
target_include_directories(kioku PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable( kioku-server
                src/main.c
                extern/mongoose/mongoose.c
                extern/parson/parson.c
                )
target_link_libraries( kioku-server kioku ${KIOKU_LIBS})
add_dependencies( kioku-server kioku )

# add_subdirectory(src)
if (BUILD_TESTS)
   enable_testing()
   add_subdirectory(test)

   #set( CTEST_MEMORYCHECK_COMMAND valgrind )
   #set( CTEST_MEMORYCHECK_COMMAND_OPTIONS "--tool=callgrind -v" )
   add_test(NAME TestFileSystem COMMAND filesystem)
   add_test(NAME TestGit COMMAND git)
   add_test(NAME TestModel COMMAND model)
endif()
