// TODO:
//   Numbers
//   Strings
//   Booleans
//   Lists
//   I/O

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id.h"

struct list {
	struct node *head;
};

struct node {
	struct node *next;
	struct object *value;
};

struct vtable *list_vt;

// This overrides object_vt's allocate
struct object *new_list() {
	return send(list_vt, s_allocate, sizeof(struct list));
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

// Print an object's address and content as a string.
struct object *str_print(struct object *self) {
	printf("%p: %s\n", self, (char *) self);
	return self;
}

int main(int argc, char **argv) {
	init();

	// Delegate a VT from object_vt for lists
	list_vt = (struct vtable *) send(object_vt, s_delegated);
	send(list_vt, s_addMethod, s_allocate, new_list);

	struct object *s_add = send(symbol, s_intern, (struct object *)"add");
	send(list_vt, s_addMethod, s_add, list_add);
	// Replace its allocate
	send(list_vt, s_allocate, new_list);

	char *test_string  = (char *) send(object_vt, s_allocate, 20);
	strcpy(test_string, "Testing.");
	char *test_string2 = (char *) send(object_vt, s_allocate, 20);
	strcpy(test_string2, "Testing again.");

	struct object *l = send(list_vt, s_allocate);
	send(l, s_add, test_string);
	send(l, s_add, test_string2);

	return 0;
}
