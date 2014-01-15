#include "id.h"

struct object *Long;

struct object *new_long(struct object *self, unsigned long num) {
	unsigned long *l = (unsigned long *) send(
			self->_vt[-1], allocate, sizeof(unsigned long));
	*l = num;
	return (struct object *) l;
}

struct object *initLong() {
	// Create a Number class.
	struct vtable *long_vt =
			(struct vtable *) send(object_vt, delegated);
	send(long_vt, addMethod, new, new_long); // Static
	struct object *Long = send(long_vt, allocate, 0);

	return Long;
}
