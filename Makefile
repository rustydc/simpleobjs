CC=clang
CFLAGS=-g -Wall -Werror -Wno-array-bounds
OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))
HEADERS=$(wildcard *.h)

id: $(OBJECTS)
id.o: id.c $(HEADERS)

clean:
	rm -f id $(OBJECTS)
