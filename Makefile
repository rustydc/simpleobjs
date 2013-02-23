CC=clang
CFLAGS=-ggdb -Wall -Werror -Wno-array-bounds
OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))
HEADERS=$(wildcard *.h)

all: test_destroy test_stdlib
test_destroy: id.o test_destroy.o
test_stdlib: id.o test_stdlib.o
	$(CC) test_stdlib.o id.o -lm -o test_stdlib
%.o: %.c $(HEADERS)

clean:
	rm -f test_destroy test_stdlib $(OBJECTS)

run: clean test_stdlib
	./test_stdlib
