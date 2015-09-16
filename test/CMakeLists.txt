if(TARGET_LIKE_MBED_CLIENT_LINUX_UNIT_TESTS)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include /usr/include/CppUTest/MemoryLeakDetectorMallocMacros.h -include /usr/include/CppUTest/MemoryLeakDetectorNewMacros.h")
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


