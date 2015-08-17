include ../makefile_defines.txt

COMPONENT_NAME = m2mconnectionhandlerimpl_linux_unit
SRC_FILES = \
        ../../../../../lwm2m-client-linux/source/m2mconnectionhandler.cpp \
        ../../../../../lwm2m-client-linux/source/connthreadhelper.cpp

TEST_SRC_FILES = \
	main.cpp \
        m2mconnectionhandlerimpl_linuxtest.cpp \
        test_m2mconnectionhandlerimpl_linux.cpp \
       ../stub/m2mconnectionhandlerpimpl_stub.cpp \
       ../stub/common_stub.cpp \
       ../stub/nsdlaccesshelper_stub.cpp \
       ../stub/m2mstring_stub.cpp \
       ../stub/m2mconnectionsecurity_stub.cpp \


include ../MakefileWorker.mk

