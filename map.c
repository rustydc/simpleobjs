#include <stdio.h>
#include <stdlib.h>
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
	float loadFactor;
	struct entry* entries;
};

struct object *new_map(struct object *self);
struct object *map_get(struct map *self, struct object *key);
void map_insert(struct map *self, struct object *key, struct object *val);
void map_double(struct map *self);

struct object *new_map(struct object *self) {
	struct map *m = (struct map *) send(
			self->_vt[-1], allocate, sizeof(struct map));
	m->entries = calloc(16, sizeof(struct entry));
	m->capacity = 16;
	m->loadFactor = 1;
	return (struct object *) m;
}

struct object *map_get(struct map *self, struct object *key) {
	int bucket, startBucket, i;
	struct object *hash = send(
			symbol, intern, "hash");
	unsigned long *keyHash = (unsigned long *) send(key, hash);
	startBucket = *keyHash % self->capacity;
	for (i = 0; i != self->capacity; i++) {
		bucket = (startBucket + i) % self->capacity;
		struct entry *curEntry = &self->entries[bucket];
		if (curEntry->key == NULL) {
			return NULL;
		} else if (curEntry->hash == *keyHash &&
				curEntry->key == key) {
			return curEntry->val;
		}
	}
	return NULL;
}

void map_insert(struct map *self, struct object *key, struct object *val) {
	int bucket, startBucket, i;

	if (self->length >= self->loadFactor * self->capacity) {
		map_double(self);
	}
	
	struct object *hash = send(
			symbol, intern, (struct object *) "hash");
	unsigned long *keyHash = (unsigned long *) send(key, hash);
	startBucket = *keyHash % self->capacity;
	for (i = 0; i != self->capacity; i++) {
		bucket = (startBucket + i) % self->capacity;
		// TODO: Check key equality.
		if (self->entries[bucket].key == NULL) {
			// Found an empty slot, insert.
			self->entries[bucket].hash = *keyHash;
			self->entries[bucket].key = key;
			self->entries[bucket].val = val;
			self->length++;
			return;
		} else if (self->entries[bucket].hash == *keyHash && self->entries[bucket].key == key) {
			// Replace it.
			self->entries[bucket].val = val;
			return;
		}
	}

	// No empty slots found; not possible.
	assert(0); 
}

void map_double(struct map *self) {
	int i;
	struct entry* oldEntries = self->entries;
	int oldCapacity = self->capacity;

	self->capacity *= 2;
	self->length = 0;
	self->entries = calloc(sizeof(struct entry), self->capacity);

	for (i = 0; i != oldCapacity; i++) {
		if (oldEntries[i].key != NULL) {
			map_insert(self, oldEntries[i].key, oldEntries[i].val);
		}
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
