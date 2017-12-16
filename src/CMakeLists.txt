add_library( kioku ${LIBRARY_TYPE}
                   kioku/error.c
                   kioku/log.c
                   kioku/datastructure.c
                   kioku/filesystem.c
                   kioku/git.c
                   kioku/schedule.c
                   kioku/string.c
                   kioku/model.c
                   kioku/model/card.c
                   kioku/controller.c
                   kioku/rest.c
                   kioku/server.c
                   ${CMAKE_SOURCE_DIR}/extern/parson/parson.c
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
