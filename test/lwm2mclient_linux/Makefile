PLATFORM= 
OS = LINUX
TARGET	= lwm2mclient_linux
OBJECTS = main.o
CFLAGS	= -std=c++11 -Wall -D_REENTRANT -D$(OS) -I ../../lwm2m-client -I ../../source/include -I ../../../libService/libService -I ../../ -DTARGET_LIKE_LINUX -DHAVE_DEBUG
LDFLAGS = -D_REENTRANT -L../../ -llwm2m_gcc -L ../../../nsdl-c -lnsdl_gcc -L ../../../libService -llist_gcc -lTrace_gcc -lip6string_gcc -lbits_gcc -lpthread
all: $(TARGET) 

$(TARGET): $(OBJECTS)
	$(PLATFORM)g++ -g -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
.cpp.o:
	$(PLATFORM)g++ -c -g -O2 $(CFLAGS) $< 
	
clean:
	rm -f $(TARGET) $(OBJECTS)

