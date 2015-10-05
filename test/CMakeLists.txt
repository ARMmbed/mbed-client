if(TARGET_LIKE_X86_LINUX_NATIVE_COVERAGE)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include /usr/include/CppUTest/MemoryLeakDetectorMallocMacros.h -include /usr/include/CppUTest/MemoryLeakDetectorNewMacros.h -D__thumb2__ -w")
include_directories($ENV{CPPUTEST_HOME}/include)
link_directories($ENV{CPPUTEST_HOME}/lib) 
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      if(EXISTS "${curdir}/${child}/CMakeLists.txt")
        LIST(APPEND dirlist ${curdir}/${child})
      endif()
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR}/mbedclient/utest)

FOREACH(subdir ${SUBDIRS})
    ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()

endif()


