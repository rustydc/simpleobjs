#include <stdio.h>
#include "id.h"

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
