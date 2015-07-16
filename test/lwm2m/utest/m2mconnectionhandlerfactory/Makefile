include ../makefile_defines.txt

COMPONENT_NAME = m2mconnectionhandlerfactory_unit
SRC_FILES = \
        ../../../../source/m2mconnectionhandlerfactory.cpp

TEST_SRC_FILES = \
        main.cpp \
        m2mconnectionhandlerfactorytest.cpp \
        test_m2mconnectionhandlerfactory.cpp \
       ../stub/m2mconnectionhandler_stub.cpp \
       ../stub/m2mstring_stub.cpp \
       ../stub/m2mconnectionsecurity_stub.cpp \

CPPUTEST_ADDITIONAL_CXXFLAGS = -DUSE_LINUX

include ../MakefileWorker.mk

