CC = g++
CFLAGS=-g -Wall -std=c++98 -Werror -fno-stack-protector -I./ -I./libs/jsoncpp/include
LDFLAGS=-pthread

BINARY=server
JSONCPP=libs/jsoncpp/jsoncpp.a
LIBPOOL=libs/nps-tool/libpool.a
OBJECTS=embarcacion.o elemento.o casilla.o tablero.o globales.o jugador.o modelo.o jsonificador.o decodificador.o RWLock.o

.PHONY: clean all libs

all: $(OBJECTS) libs server

$(BINARY): server.c $(OBJECTS) $(JSONCPP) $(LIBPOOL)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

$(JSONCPP):
	$(MAKE) -C libs/jsoncpp/src

$(LIBPOOL):
	$(MAKE) -C libs/nps-tool
	
clean:
	rm -f $(OBJECTS)
	rm -f $(BINARY)
