#include <stdio.h>
#include <string.h>
#include "id.h"
#include "stdlib/map.h"
#include "stdlib/string.h"

int main(int argc, char **argv) {
	init();

	// Create some symbols.
	struct object *new   = send(symbol, intern, (struct object *) "new"  );
	struct object *put   = send(symbol, intern, (struct object *) "put"  );
	struct object *get   = send(symbol, intern, (struct object *) "get"  );
	struct object *print = send(symbol, intern, (struct object *) "print");

	// Initialize classes.
	struct object *Map    = initMap();
	struct object *String = initString();

	// Create some objects.
	struct object *map = send(Map, s_new);
	struct object *testString1 = send(String, new, "Testing."      );
	struct object *testString2 = send(String, new, "Testing again.");

	send(map, put, testString1, testString2);
	send(map, put, testString2, testString2);
	send(map, put, testString2, testString1);

	send(send(map, get, testString2), print);
	send(send(map, get, testString1), print);

	return 0;
}
