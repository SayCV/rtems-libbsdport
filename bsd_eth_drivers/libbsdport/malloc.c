#include <stdlib.h>

/* sometimes we want the original versions,
 * not malloc/free shadowed by rtems' bsdnet port
 */

void *
real_libc_malloc(size_t s)
{
	return malloc(s);
}

void
real_libc_free(void *p)
{
	free(p);
}
