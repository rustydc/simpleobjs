// Based heavily on Ian Piumarta's ID object model.
// http://piumarta.com/software/id-objmodel/objmodel2.pdf

// IDEAS:
//   Needs a language.
//   Expose as library?
//   Modularize globals?
//   Interfaces? Traits? Typeclasses?
//   Garbage collection?
//   Network support?
//     Via proxy object?
//     Serialization?
//     Garbage collection over network?
//   Expose HTTP for sends?
//     Interaction with JS maybe??
//   Async?
//   Data types?
//     Tuples
//     Array?
//     Ints, floats, bigints, doubles, rationals...
//   AST for this language?
//     EXPR = [ASSIGN] OBJECT [MSG ARGS ...]
//     Objects, variables? Scoping?
//     Then interpreter. (JIT?)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vtable;
struct object;
struct symbol;

// This takes a receiver and a list of params, and returns an object
typedef struct object *(*method_t)(struct object *receiver, ...);

struct object {
	struct vtable *_vt[0];
	void *data;
};

struct vtable {
	struct vtable *_vt[0];
	int size;
	int tally;
	struct object **keys;
	struct object **values;
	struct vtable *parent;
};

struct symbol {
	struct vtable *_vt[0];
	char *string;
};

struct vtable *vtable_vt;
struct vtable *object_vt;
struct vtable *symbol_vt;
struct object *s_addMethod;
struct object *s_allocate;
struct object *s_delegated;
struct object *s_lookup;
struct object *s_intern;
struct object *symbol;
struct vtable *symbol_list;

struct object *vtable_lookup(struct vtable *self, struct object *key);
struct object *symbol_new(char *string);

#define send(RCV, MSG, ARGS...) ({                 \
	struct object *r = (struct object *)(RCV); \
	method_t method = _bind(r, (MSG));         \
	method(r, ##ARGS);                         \
})

method_t _bind(struct object *rcv, struct object *msg) {
	method_t method;
	struct vtable *vt = rcv->_vt[-1];
	method = ((msg == s_lookup) && (rcv == (struct object *) vtable_vt))
	       ? (method_t) vtable_lookup(vt, msg)
	       : (method_t) send(vt, s_lookup, msg);
	return method;
}

// 'calloc' a block of size bytes, plus space for a pointer before it.
void *alloc(size_t size) {
	struct vtable **ppvt =
	        (struct vtable **) calloc(1, sizeof(struct vtable *) + size);
	return (void *)(ppvt + 1);
}

// Create a new object for a given vtable.
struct object *vtable_allocate(struct vtable *self, int payload_size) {
	struct object *object = (struct object *) alloc(payload_size);
	object->_vt[-1] = self;
	return object;
}

// Create a new vtable with parent set to 'self'
struct vtable *vtable_delegated(struct vtable *self) {
	struct vtable *child =
                (struct vtable *) vtable_allocate(self, sizeof(struct vtable));
	child->_vt[-1] = self ? self->_vt[-1] : 0;
	child->size = 2;
	child->tally = 0;
	child->keys   =
	        (struct object **) calloc(child->size, sizeof(struct object *));
	child->values =
	        (struct object **) calloc(child->size, sizeof(struct object *));
	child->parent = self;
	return child;
}

// Add a method to a vtable.
struct object *vtable_addMethod(struct vtable *self, struct object *key, 
                                struct object *method)
{
	int i;
	for (i = 0; i < self->tally; i++) {
		if (key == self->keys[i]) {
			return self->values[i] = (struct object *) method;
		}
	}

	// Too big, double the vtable's size.
	if (self->tally == self->size) {
		self->size *= 2;
		self->keys   = (struct object **) realloc(self->keys, sizeof(struct object *) * self->size);
		self->values = (struct object **) realloc(self->values, sizeof(struct object *) * self->size);
	}

	// Add the new key, value, and increase tally.
	self->keys  [self->tally  ] = key;
	self->values[self->tally++] = method;
	return method;
}

// 
struct object *vtable_lookup(struct vtable *self, struct object *key) {
	// Check each entry for that exact key, by address.
	int i;
	for (i = 0; i != self->tally; i++) {
		if (key == self->keys[i]) {
			return self->values[i];
		}
	}

	// Check our parent if we have one.
	if (self->parent) {
		return send(self->parent, s_lookup, key);
	}

	// Lookup failed.
	printf("Lookup failure: %s\n", ((struct symbol *) key)->string);
	return 0;
}

// Create a symbol
struct object *symbol_intern(struct object *self, char *string) {
	struct object *symbol;
	int i;
	for (i = 0; i < symbol_list->tally; i++) {
		symbol = symbol_list->keys[i];
		if (!strcmp(string, ((struct symbol *)symbol)->string)) {
			// Found it.
			return symbol;
		}
	}
	symbol = symbol_new(string);
	vtable_addMethod(symbol_list, symbol, 0);
	return symbol;
}

// Create an object that is a string.
struct object *symbol_new(char *string) {
	struct symbol *symbol = (struct symbol *) alloc(sizeof(struct symbol));
	symbol->_vt[-1] = symbol_vt;
	symbol->string = strdup(string);
	return (struct object *) symbol;
}

// Bootstrap the object system.
void init() {
	// vtable_vt is its own vtable.
	vtable_vt = vtable_delegated(0);
	vtable_vt->_vt[-1] = vtable_vt;

	// object has vtable_vt as its vtable
	object_vt = vtable_delegated(0);
	object_vt->_vt[-1] = vtable_vt;
	// and vtable_vt is an object
	vtable_vt->parent = object_vt;

	symbol_vt = vtable_delegated(object_vt);
	// symbol_list is just an orphan vtable
	symbol_list = vtable_delegated(0);

	s_lookup = symbol_intern(0, "lookup");
	vtable_addMethod(vtable_vt, s_lookup, (struct object *)vtable_lookup);
	// send and bind should now work.

	s_addMethod = symbol_intern(0, "addMethod");
	vtable_addMethod(vtable_vt, s_addMethod,
	        (struct object *) vtable_addMethod);
	// addMethod should now work.

	s_allocate = symbol_intern(0, "allocate");
	send(vtable_vt, s_addMethod, s_allocate, vtable_allocate);
	// allocate should now create a new member of an object family

	symbol = send(symbol_vt, s_allocate, sizeof(struct symbol));

	s_intern = symbol_intern(0, "intern");
	send(symbol_vt, s_addMethod, s_intern, symbol_intern);
	// new symbols can now be interned.

	s_delegated = send(symbol, s_intern, (struct object *)"delegated");
	send(vtable_vt, s_addMethod, s_delegated, vtable_delegated);
	// new vtables can now be created.
}

// Print an object (with its address) as a string.
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
