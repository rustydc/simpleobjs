#ifndef __ID_H__
#define __ID_H__

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
method_t _bind(struct object *rcv, struct object *msg);
void init();

#define send(RCV, MSG, ARGS...) ({                 \
	struct object *r = (struct object *)(RCV); \
	method_t method = _bind(r, (MSG));         \
	method(r, ##ARGS);                         \
})

#endif
