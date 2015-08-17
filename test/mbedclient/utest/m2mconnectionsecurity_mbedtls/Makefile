include ../makefile_defines.txt

COMPONENT_NAME = m2mconnectionsecurity_unit
SRC_FILES = \
        ../../../../../lwm2m-client-mbedtls/source/m2mconnectionsecurity.cpp \

TEST_SRC_FILES = \
	main.cpp \
	m2mconnectionsecurity_mbedtlstest.cpp \
        test_m2mconnectionsecurity_mbedtls.cpp \
        ../stub/m2mstring_stub.cpp \
        ../stub/m2mtimer_stub.cpp \
        ../stub/m2mbase_stub.cpp \
        ../stub/m2mobject_stub.cpp \
        ../stub/m2msecurity_stub.cpp \
        ../stub/mbedtls_stub.cpp \
        ../stub/m2mconnectionhandler_stub.cpp \
        ../stub/m2mconnectionsecuritypimpl_stub.cpp \


include ../MakefileWorker.mk

