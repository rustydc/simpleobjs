#include <assert.h>
#include "id.h"

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
	struct map *m = (struct map *) send(
			self->_vt[-1], allocate, sizeof(struct map));
	m->entries = calloc(16, sizeof(struct entry));
	m->capacity = 16;
	return (struct object *) m;
}

struct object *map_get(struct map *self, struct object *key) {
	int bucket, startBucket, i;
	struct object *hash = send(
			symbol, intern, (struct object *) "new");
	struct num *hash = (struct num *) send(key, hash);
	startBucket = hash->number % self->capacity;
	for (i = 0; i != self->capacity; i++) {
		bucket = startBucket + i % self->capacity;
		struct entry *curEntry = &self->entries[bucket];
		if (curEntry->key == NULL) {
			return NULL;
		} else if (curEntry->hash == hash->number &&
		           curEntry->key == key) {
			return curEntry->val;
		}
	}
	return NULL;
}

void map_insert(struct map *self, struct object *key, struct object *val) {
	int bucket, startBucket, i;

	if (self->length >= 0.75 * self->capacity) {
		map_double(self);
	}
	
	struct object *hash = send(
			symbol, intern, (struct object *) "new");
	struct num *hash = (struct num *) send(key, hash);
	startBucket = hash->number % self->capacity;
	for (i = 0; i != self->capacity; i++) {
		bucket = (startBucket + i) % self->capacity;
		struct entry *curEntry = &self->entries[bucket];
		// TODO: Check key equality.
		if (curEntry->key == NULL) {
			// Found an empty slot, insert.
			curEntry->hash = hash->number;
			curEntry->key = key;
			curEntry->val = val;
			self->length++;
			return;
		} else if (curEntry->hash == hash->number && curEntry->key == key) {
			// Replace it.
			curEntry->val = val;
			return;
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

struct object *initMap() {
	struct object *new = send(symbol, intern, (struct object *) "new");
	struct object *put = send(symbol, intern, (struct object *) "put");
	struct object *get = send(symbol, intern, (struct object *) "get");

	// Create a Hashmap class.
	struct vtable *hashmap_vt = (struct vtable *) send(object_vt, delegated);
	send(hashmap_vt, addMethod, new, new_map); // Static
	send(hashmap_vt, addMethod, put, map_insert);
	send(hashmap_vt, addMethod, get, map_get);
	struct object *Hashmap = send(hashmap_vt, allocate, 0);

	return Hashmap;
}