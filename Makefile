CC=clang
CFLAGS=-g -Wall -Werror -Wno-array-bounds
OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))
HEADERS=$(wildcard *.h)

all: test_destroy
test_destroy: $(OBJECTS)
%.o: %.c $(HEADERS)

clean:
	rm -f test_destroy $(OBJECTS)
