#ifndef __ID_H__
#define __ID_H__

#include <string.h>

struct vtable;
struct object;
struct symbol;

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

extern struct vtable *vtable_vt;
extern struct vtable *object_vt;
extern struct object *addMethod;
extern struct object *allocate;
extern struct object *delegated;
extern struct object *lookup;
extern struct object *intern;
extern struct object *symbol;
extern struct object *new;
struct vtable *symbol_vt;
struct vtable *symbol_list;

// A method takes a receiver and a list of params, and returns an object
typedef struct object *(*method_t)(struct object *receiver, ...);

method_t _bind(struct object *rcv, struct object *msg);
void init();

#define send(RCV, MSG, ARGS...) ({                 \
	struct object *r = (struct object *)(RCV); \
	method_t method = _bind(r, (MSG));         \
	method(r, ##ARGS);                         \
})

#endif
