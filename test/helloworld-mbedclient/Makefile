PLATFORM= 
OS = LINUX
TARGET	= helloworld_mbedclient
OBJECTS = main.o mbedclient.o to_be_ported.o
CFLAGS	= -std=c++11 -Wall -D_REENTRANT -D$(OS) -I ../../lwm2m-client -I ../../../lwm2m-client-mbedtls \
	  -I ../../source/include -I ../../../../libService/libService -I ../../../../mbed-trace \
  	  -I ../../ -I ../../../ -DTARGET_LIKE_LINUX -DHAVE_DEBUG

LDFLAGS = -D_REENTRANT -L../../ -lmbedclient_gcc -L../../../lwm2m-client-linux -lmbed-client-linux_gcc -L../../../lwm2m-client-mbedtls -lmbed-client-mbedtls_gcc \
			-L ../../../../nsdl-c -lnsdl_gcc \
			-L ../../../../libService -lservice_gcc -L ../../../../mbed-trace -lmbed-trace_gcc -lpthread -lrt \
			-L../../../../mbedtls/library -lmbedtls -lmbedcrypto -lmbedx509

all: $(TARGET) 

$(TARGET): $(OBJECTS)
	$(PLATFORM)g++ -g -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
.cpp.o:
	$(PLATFORM)g++ -c -g -O2 $(CFLAGS) $< 
	
clean:
	rm -f $(TARGET) $(OBJECTS)

