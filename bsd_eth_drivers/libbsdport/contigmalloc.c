#include <rtems.h>

#define _KERNEL
#include <rtems/rtems_bsdnet_internal.h>
#include <sys/malloc.h>

void *
contigmalloc(
	unsigned long size,
	int type,
	int flags,
	unsigned long lo,
	unsigned long hi,
	unsigned long align,
	unsigned long bound)
{
void *ptr  = rtems_bsdnet_malloc(size + sizeof(ptr) + align-1, type, flags);
char *rval = 0;
	if ( ptr ) {
		unsigned tmp = (unsigned)ptr + align - 1;
		tmp -= tmp % align;
		rval = (char*)tmp;
		/* save backlink */
		*(void**)(rval+size) =  ptr;
	}
	return rval;
}

void
contigfree(void *ptr, size_t size, int type)
{
	rtems_bsdnet_free( *(void**)((unsigned)ptr + size), type);
}
