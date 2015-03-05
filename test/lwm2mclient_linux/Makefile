PLATFORM= 
OS = LINUX
TARGET	= lwm2mclient_linux
OBJECTS = main.o
CFLAGS	= -Wall -D_REENTRANT -D$(OS) -I ../../include -I ../../lwm2m-client -I ../../source/include -I ../../ -I ../../../nsdl-c -I ../../../../libService/include -DHAVE_DEBUG -DUSE_LINUX
LDFLAGS = -D_REENTRANT -lpthread -L ../../ -llwm2m
all: $(TARGET) 

$(TARGET): $(OBJECTS)
	$(PLATFORM)g++ -g -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
.cpp.o:
	$(PLATFORM)g++ -c -g -O2 $(CFLAGS) $< 
	
clean:
	rm -f $(TARGET) $(OBJECTS)

