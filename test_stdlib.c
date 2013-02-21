// TODO:
//   Numbers
//   Strings
//   Booleans
//   Lists
//   I/O

#include <assert.h>
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
	unsigned long hash;
	struct object *key;
	struct object *val;
};

struct map {
	int length;
	int capacity;
	struct entry* entries;
};

struct object *new_map(struct object *self) {
	struct map *m = (struct map *) send(self->_vt[-1], s_allocate, sizeof(struct map));
	m->entries = calloc(16, sizeof(struct entry));
	m->capacity = 16;
	return (struct object *) m;
}

void map_double(struct map *self);

long str_hash(char *str) {
	long hash = 0, i = 0;
	while (str[i] != '\0') {
		hash = hash * 127 + str[i]; 
		i++;
	}
	return hash;
}

struct object *map_get(struct map *self, struct object *key) {
	int bucket, startBucket, i;
	long hash;
	hash = str_hash((char *) key);
	startBucket = hash % self->capacity;
	for (i = 0; i != self->capacity; i++) {
		bucket = startBucket + i % self->capacity;
		if (self->entries[bucket].key == NULL) {
			return NULL;
		} else if (self->entries[bucket].key == key) {
			return self->entries[bucket].val;
		}
	}
	return NULL;
}

void map_insert(struct map *self, struct object *key, struct object *val) {
	int bucket, startBucket, i;
	unsigned long hash;

	if (self->length >= 0.75 * self->capacity) {
		map_double(self);
	}
	
	// TODO: Send hash message.
	hash = str_hash((char *) key);
	startBucket = hash % self->capacity;
	printf("h=%lu, b=%d\n", hash, startBucket);
	for (i = 0; i != self->capacity; i++) {
		bucket = (startBucket + i) % self->capacity;
		struct entry *curEntry = &self->entries[bucket];
		// TODO: Check key equality.
		if (curEntry->key == NULL) {
			// Found an empty slot, insert.
			printf(" Empty: %d\n", i);
			curEntry->hash = hash;
			curEntry->key = key;
			curEntry->val = val;
			self->length++;
			return;
		} else if (curEntry->hash == hash && curEntry->key == key) {
			// Replace it.
			curEntry->val = val;
			return;
		} else {
			printf(" Occupied.\n");
		}
	}

	// No empty slots found; not possible.
	assert(0); 
}

void map_double(struct map *self) {
	int i;
	struct entry* oldEntries = self->entries;

	assert(self->capacity == self->length);

	self->capacity *= 2;
	self->entries = calloc(sizeof(struct entry), self->capacity);

	for (i = 0; i != self->length; i++) {
		map_insert(self, oldEntries[i].key, oldEntries[i].val);
	}

	free(oldEntries);
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

void vtable_print(struct vtable* self) {
	int i;
	printf("Printing %p:\n", self);
	for (i = 0; i != self->tally; i++) {
		printf(" %02d: %s -> %p\n", i, ((struct symbol *)self->keys[i])->string, self->values[i]);
	}
	if (self->parent) {
		printf("parent: %p\n", self->parent);
		vtable_print(self->parent);
	}
}

int main(int argc, char **argv) {
	init();

	struct object *s_new = send(symbol, s_intern, (struct object *) "new");
	struct object *s_put = send(symbol, s_intern, (struct object *) "put");
	struct object *s_get = send(symbol, s_intern, (struct object *) "get");
	struct object *s_print = send(symbol, s_intern, (struct object *) "print");

	hashmap_vt = (struct vtable *) send(object_vt, s_delegated);
	send(hashmap_vt, s_addMethod, s_new, new_map);
	send(hashmap_vt, s_addMethod, s_put, map_insert);
	send(hashmap_vt, s_addMethod, s_get, map_get);
	// Create a Hashmap class.
	struct object *Hashmap = send(hashmap_vt, s_allocate, 0);

	// Create an instance.
	struct object *hashmap = send(Hashmap, s_new);


	send(object_vt, s_addMethod, s_print, str_print);

	char *test_string  = (char *) send(object_vt, s_allocate, 20);
	strcpy(test_string, "Testing.");
	char *test_string2 = (char *) send(object_vt, s_allocate, 20);
	strcpy(test_string2, "Testing again.");

	send(hashmap, s_put, test_string, test_string2);
	send(hashmap, s_put, test_string2, test_string);
	send(hashmap, s_put, test_string2, test_string);

	send(send(hashmap, s_get, test_string2), s_print);
	send(send(hashmap, s_get, test_string), s_print);
	printf("Hashmap size: %d\n", ((struct map *)hashmap)->length);

	return 0;
}
