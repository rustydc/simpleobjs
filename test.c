#include <stdio.h>
#include <string.h>
#include "id.h"
#include "map.h"
#include "string.h"
#include "long.h"

int main(int argc, char **argv) {
	init();

	struct object *Long = initLong();
	struct object *String = initString(Long);
	//struct object *k = initKernel();

	// Create some symbols.
	struct object *new   = send(symbol, intern, (struct object *) "new"  );
	struct object *put   = send(symbol, intern, (struct object *) "put"  );
	//struct object *get   = send(symbol, intern, (struct object *) "get"  );
	//struct object *print = send(symbol, intern, (struct object *) "print");

	// Initialize classes.
	struct object *Map    = initMap();

	// Create some objects.
	struct object *map = send(Map, new);
	struct object *strings[500000];
	int i;
	char *str = malloc(50);
	for (i = 0; i != 500000; i++) {
		sprintf(str, "%d: A string.", i);
		strings[i] = send(String, new, str);
		send(map, put, strings[i], strings[i]);
	}

	struct object *testString1 = send(String, new, "Testing."      );
	struct object *testString2 = send(String, new, "Testing again.");

	send(map, put, testString1, testString2);
	send(map, put, testString2, testString2);
	send(map, put, testString2, testString1);

	//send(send(map, get, testString2), print);
	//send(send(map, get, testString1), print);

	return 0;
}
