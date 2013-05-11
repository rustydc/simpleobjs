CC=gcc
CFLAGS=-g -Wall -Werror -Wno-array-bounds
OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))
HEADERS=$(wildcard *.h)

all: test
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -c -o $@
test: id.o test.o $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -lm -o test

clean:
	rm -f test $(OBJECTS)

run: clean test
	./test
