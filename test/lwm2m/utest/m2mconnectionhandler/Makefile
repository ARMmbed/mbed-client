include ../makefile_defines.txt

COMPONENT_NAME = m2mconnectionhandler_unit
SRC_FILES = \
        ../../../../source/m2mconnectionhandler.cpp

TEST_SRC_FILES = \
	main.cpp \
       ../stub/m2mconnectionhandlerimpl_mbed_stub.cpp \
       ../stub/m2mstring_stub.cpp \
        m2mconnectionhandlertest.cpp \
        test_m2mconnectionhandler.cpp

CPPUTEST_ADDITIONAL_CXXFLAGS = -DUSE_LINUX

include ../MakefileWorker.mk

