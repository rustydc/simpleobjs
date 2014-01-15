#include <stdio.h>
#include <string.h>
#include "id.h"
#include "long.h"

struct object *Long = NULL;

//struct object *string_equals(struct object *self, struct object *other) {
//	return NULL;
	//self->_vt[-1] == other->_vt[-1] &&
	//		!strcmp((char *)self, (char *)other);
//}

struct object *new_string(struct object *self, char *str) {
	struct object *s = send(self->_vt[-1], allocate, strlen(str) + 1);
	strcpy((char *)s, str);
	return s;
}

unsigned long *string_hash(char *str) {
	unsigned long hash = 0, i = 0;
	while (str[i] != '\0') {
		hash = hash * 31 + str[i]; 
		i++;
	}
	unsigned long *l = (unsigned long *) send(Long, new);
	*l = hash;
	return l;
}
	
// Print an object's address and content as a string.
struct object *string_print(struct object *self) {
	printf("%p: %s\n", self, (char *) self);
	return self;
}

struct object *initString(struct object *LongRef) {
	Long = LongRef;
	struct object *new   = send(symbol, intern, (struct object *) "new"  );
	struct object *print = send(symbol, intern, (struct object *) "print");
	struct object *hash  = send(symbol, intern, (struct object *) "hash" );

	// Create a String class.
	struct vtable *string_vt = (struct vtable *) send(object_vt, delegated);
	send(string_vt, addMethod, new, new_string); // Static
	send(string_vt, addMethod, print, string_print);
	send(string_vt, addMethod, hash, string_hash);
	struct object *String = send(string_vt, allocate, 0);

	return String;
}
