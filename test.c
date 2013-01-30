#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id.h"

// Print an object's address and content as a string.
struct object *str_print(struct object *self) {
	printf("%p: %s\n", self, (char *) self);
	return self;
}

int main(int argc, char **argv) {
	init();

	char *test_string  = (char *) send(object_vt, s_allocate, 20);
	char *test_string2 = (char *) send(object_vt, s_allocate, 20);
	strcpy(test_string, "Testing.");
	strcpy(test_string2, "Testing again.");

	struct object *print = send(symbol, s_intern, (struct object *)"print");
	send(object_vt, s_addMethod, print, str_print);

	send(test_string, print);
	send(test_string2, print);

	return 0;
}
