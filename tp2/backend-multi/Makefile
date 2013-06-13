CC = g++
CFLAGS=-g -Wall -std=c++98
LDFLAGS=-pthread

BINARY=test
#JSONCPP=libs/jsoncpp/jsoncpp.a
OBJECTS=TestLuciano.o RWLock.o

.PHONY: clean all libs

all: $(OBJECTS) test

$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

#$(JSONCPP):
#	$(MAKE) -C libs/jsoncpp/src

clean:
	rm -f $(OBJECTS)
	rm -f $(BINARY)
