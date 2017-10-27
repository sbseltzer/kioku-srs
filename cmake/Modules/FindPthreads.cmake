#.rst:
# FindPthreads
# ------------
#
# Try to find pthreads libraries.
# https://sourceware.org/pthreads-win32/
#
# You may declare PTHREADS_ROOT environment variable to tell where
# your library is installed. 
#
# Once done this will define::
#
#   Pthreads_FOUND          - True if pthreads was found
#   Pthreads_INCLUDE_DIRS   - include directories for pthreads
#   Pthreads_LIBRARIES      - link against this library to use pthreads
#
# The module will also define two cache variables::
#
#   Pthreads_INCLUDE_DIR    - the pthreads include directory
#   Pthreads_LIBRARY        - the path to the pthreads library
#

#=============================================================================
# Copyright 2014 Mikael Lepistö
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# This file is from https://raw.githubusercontent.com/elhigu/cmake-findpthreads/master/cmake/FindPthreads.cmake

if(WIN32)
  find_path(Pthreads_INCLUDE_DIR
    NAMES
      pthread.h
    PATHS
      ENV "PROGRAMFILES(X86)"
      ENV PTHREADS_ROOT
    PATH_SUFFIXES
      include
  )

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIB_PATH lib/x64)
  else()
    set(LIB_PATH lib/x86)
  endif()

  find_library(Pthreads_LIBRARY
    NAMES 
      pthread.lib
      pthreadVC2.lib
      pthreadVC2.lib
    PATHS
      ENV PTHREADS_ROOT
    PATH_SUFFIXES
      ${LIB_PATH}
  )
  
  #
  # All good
  #

  set(Pthreads_LIBRARIES ${Pthreads_LIBRARY})
  set(Pthreads_INCLUDE_DIRS ${Pthreads_INCLUDE_DIR})

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(
    Pthreads
    FOUND_VAR Pthreads_FOUND
    REQUIRED_VARS Pthreads_LIBRARY Pthreads_INCLUDE_DIR
    VERSION_VAR Pthreads_VERSION_STRING)

  mark_as_advanced(
    Pthreads_INCLUDE_DIR
    Pthreads_LIBRARY)

else()

  message(ERROR "IMPLEMENTED ONLY IN WINOWS YET")
  # Find with pkgconfig
  find_package(PkgConfig)

  if(HWLOC_ROOT)
    set(ENV{PKG_CONFIG_PATH} "${HWLOC_ROOT}/lib/pkgconfig")
  else()
    foreach(PREFIX ${CMAKE_PREFIX_PATH})
      set(PKG_CONFIG_PATH "${PKG_CONFIG_PATH}:${PREFIX}/lib/pkgconfig")
    endforeach()
    set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH}")
  endif()

  if(hwloc_FIND_REQUIRED)
    set(_hwloc_OPTS "REQUIRED")
  elseif(hwloc_FIND_QUIETLY)
    set(_hwloc_OPTS "QUIET")
  else()
    set(_hwloc_output 1)
  endif()

  if(hwloc_FIND_VERSION)
    if(hwloc_FIND_VERSION_EXACT)
      pkg_check_modules(HWLOC ${_hwloc_OPTS} hwloc=${hwloc_FIND_VERSION})
    else()
      pkg_check_modules(HWLOC ${_hwloc_OPTS} hwloc>=${hwloc_FIND_VERSION})
    endif()
  else()
    pkg_check_modules(HWLOC ${_hwloc_OPTS} hwloc)
  endif()

  if(HWLOC_FOUND)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(HWLOC DEFAULT_MSG HWLOC_LIBRARIES
      HWLOC_INCLUDE_DIRS)

    if(NOT ${HWLOC_VERSION} VERSION_LESS 1.7.0)
      set(HWLOC_GL_FOUND 1)
    endif()

    if(_hwloc_output)
      message(STATUS
        "Found hwloc ${HWLOC_VERSION} in ${HWLOC_INCLUDE_DIRS}:${HWLOC_LIBRARIES}")
    endif()
  endif()
endif()

