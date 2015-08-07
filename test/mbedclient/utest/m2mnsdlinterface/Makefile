include ../makefile_defines.txt

COMPONENT_NAME = m2mnsdlinterface_unit
SRC_FILES = \
        ../../../../source/m2mnsdlinterface.cpp \
	../../../../source/m2mconstants.cpp

CFLAGS+=-std=c++11 

TEST_SRC_FILES = \
	main.cpp \
        ../stub/m2mbase_stub.cpp \
        ../stub/m2mobject_stub.cpp \
        ../stub/m2mserver_stub.cpp \
        ../stub/m2mresource_stub.cpp \
        ../stub/m2mresourceinstance_stub.cpp \
        ../stub/m2mobjectinstance_stub.cpp \
        ../stub/m2mstring_stub.cpp \
        ../stub/m2msecurity_stub.cpp \
        ../stub/m2mtimer_stub.cpp \
        ../stub/common_stub.cpp \
        ../stub/nsdlaccesshelper_stub.cpp \
	m2mnsdlinterfacetest.cpp \
        test_m2mnsdlinterface.cpp


include ../MakefileWorker.mk

