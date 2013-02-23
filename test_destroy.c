#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id.h"

// Print an object's address and content as a string.
struct object *string_print(struct object *self) {
	printf("%p: %s\n", self, (char *) self);
	return self;
}

// Free an object's memory.
void destroy(struct object *self) {
	free(self - sizeof(void *));
}

int main(int argc, char **argv) {
	init();

	// Install a 'destroy' for all objects.
	struct object *destroy = send(symbol, intern, (struct object *)"destroy");
	send(object_vt, addMethod, destroy, destroy);

	char *testString  = (char *) send(object_vt, allocate, 20);
	char *testString2 = (char *) send(object_vt, allocate, 20);
	strcpy(testString, "Testing.");
	strcpy(testString2, "Testing again.");

	struct object *print = send(symbol, intern, (struct object *)"print");
	send(object_vt, addMethod, print, string_print);

	send(testString, print);
	send(testString2, print);

	return 0;
}
