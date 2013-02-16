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
#include "math.h"

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

struct vtable *hashmap_vt;

struct entry {
	struct object *key;
	struct object *val;
};

struct map {
	int length;
	int capacity;
	struct entry* entries;
};

int str_hash(char *str) {
	int hash = 0, i = 0;
	while (str[i] != '\0') {
		hash += str[i] * pow(256, i);
		i++;
	}
	return hash;
}

void map_insert(struct map *self, struct object *key, struct object *val) {
	int i;
	if (self->length == self->capacity) {
		int oldCapacity = self->capacity;
		struct entry* oldEntries = self->entries;

		self->capacity *= 2;
		self->entries = calloc(sizeof(struct entry), self->capacity);

		for (i = 0; i != oldCapacity; i++) {
			map_insert(self, oldEntries[i].key, oldEntries[i].val);
		}
	}

	int bucket = str_hash((char *) key) % self->capacity;
	// TODO: || key != key?
	while (self->entries[bucket].key != NULL) {
		bucket = bucket + 1 % self->capacity;
	}
	self->entries[bucket].key = key;
	self->entries[bucket].val = val;
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

	struct object *s_print = send(symbol, s_intern, (struct object *)"print");
	send(object_vt, s_addMethod, s_print, str_print);

	char *test_string  = (char *) send(object_vt, s_allocate, 20);
	strcpy(test_string, "Testing.");
	char *test_string2 = (char *) send(object_vt, s_allocate, 20);
	strcpy(test_string2, "Testing again.");

	return 0;
}
