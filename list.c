#include <assert.h>
#include <stdlib.h>
#include "id.h"

struct list {
	struct node *head;
};

struct node {
	struct node *next;
	struct object *value;
};

struct vtable *list_vt;
struct object *List;

struct object *new_list(struct object *self) {
	return send(self->_vt[-1], allocate, sizeof(struct list));
}

// For now, 'list' will be an object, but not 'node'.
void list_add(struct list *self, struct object *obj) {
	struct node *n = calloc(1, sizeof(struct node));
	n->value = obj;

	struct node *cur = self->head;
	if (cur == NULL) {
		self->head = n;
		return;
	}

	while (cur->next != NULL) {
		cur = cur->next;
	}
	cur->next = n;
}

struct object *initList() {
	struct object *add = send(symbol, intern, (struct object *) "add");

	// Create a String class.
	struct vtable *list_vt = (struct vtable *) send(object_vt, delegated);
	send(list_vt, addMethod, new, new_list); // Static
	send(list_vt, addMethod, add, list_add);
	struct object *List = send(list_vt, allocate, 0);

	return List;
}
