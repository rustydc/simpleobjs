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
	int hash;
	struct object *key;
	struct object *val;
};

struct map {
	int length;
	int capacity;
	struct entry* entries;
};

struct object *new_map() {
	struct map *m = (struct map *) send(hashmap_vt, s_allocate, sizeof(struct map));
	m->entries = calloc(16, sizeof(struct entry));
	m->capacity = 16;
	return (struct object *) m;
}

void map_double(struct map *self);

int str_hash(char *str) {
	int hash = 0, i = 0;
	
	while (str[i] != '\0') {
		hash = hash * 31 + str[i]; 
		i++;
	}
	return hash;
}

void map_insert(struct map *self, struct object *key, struct object *val) {
	int bucket, startBucket, hash, i;

	if (self->length == self->capacity) {
		map_double(self);
	}
	
	hash = str_hash((char *) key);
	startBucket = hash % self->capacity;
	for (i = 0; i != self->capacity; i++) {
		bucket = startBucket + i % self->capacity;
		if (self->entries[bucket].key == NULL) {
			// Found an empty slot, insert.
			self->entries[bucket].hash = hash;
			self->entries[bucket].key = key;
			self->entries[bucket].val = val;
			self->length++;
			return;
		} else if (self->entries[bucket].key == key) {
			// Replace it.
			self->entries[bucket].val = val;
			return;
		}
	}
	self->entries[bucket].key = key;
	self->entries[bucket].val = val;
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

int main(int argc, char **argv) {
	init();

	// Delegate a VT from object_vt for maps
	hashmap_vt = (struct vtable *) send(vtable_vt, s_delegated);
	struct object *s_new = send(symbol, s_intern, (struct object *) "new");
	send(hashmap_vt, s_addMethod, s_new, new_map);

	struct object *s_put = send(symbol, s_intern, (struct object *)"put");
	send(hashmap_vt, s_addMethod, s_put, map_insert);

	struct object *s_print = send(symbol, s_intern, (struct object *)"print");
	send(object_vt, s_addMethod, s_print, str_print);

	char *test_string  = (char *) send(object_vt, s_allocate, 20);
	strcpy(test_string, "Testing.");
	char *test_string2 = (char *) send(object_vt, s_allocate, 20);
	strcpy(test_string2, "Testing again.");

	printf("%s: %d\n", "This is a Java string", str_hash("This is a Java string"));

	//struct map *m = (struct map *) send(hashmap_vt, s_new);
	struct map *m = (struct map *) new_map();
	map_insert(m, (struct object *) test_string, (struct object *) test_string2);
	map_insert(m, (struct object *) test_string2, (struct object *) test_string);
	map_insert(m, (struct object *) test_string, (struct object *) test_string);

	return 0;
}
