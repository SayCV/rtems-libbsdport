#ifndef LIBBSDPORT_SYS_BUS_H
#define LIBBSDPORT_SYS_BUS_H

#include <rtems.h>
#include <sys/errno.h>
#include <bsp.h>
#include <devicet.h>
#include <sys/mbuf.h>

typedef uint32_t bus_addr_t;
typedef size_t   bus_size_t;

typedef enum {
	bus_space_mem = 0,
	bus_space_io  = 1
} bus_space_tag_t;

struct resource;

struct resource_spec {
	int type;
	int rid;
	int flags;
};

typedef bus_addr_t bus_space_handle_t;

/* The 'bus_space_xxx()' inlines can be helped if the
 * tag is hardcoded in the driver so that the compiler
 * can optimize part of the implementation away.
 */

#define BUS_SPACE_BARRIER_WRITE 1
#define BUS_SPACE_BARRIER_READ  2

#if defined(__i386__)

#include <rtems/score/cpu.h>

static inline void
bus_space_barrier(bus_space_tag_t t, bus_space_handle_t h, bus_size_t o, int width, int type)
{
}

#define BUS_SPACE_DECL(type, width, nwidth) \
static inline type \
bus_space_read_##nwidth(bus_space_tag_t t, bus_space_handle_t h, bus_size_t o) \
{ \
type v; \
	if ( bus_space_io == t ) { \
		/* this is a macro setting the second argument */ \
		inport_##width( h+o, v ); \
	} else { \
		v = *(volatile type __attribute__((may_alias)) *)(h+o); \
	} \
	return v; \
} \
  \
static inline void \
bus_space_write_##nwidth(bus_space_tag_t t, bus_space_handle_t h, bus_size_t o, type v) \
{ \
	if ( bus_space_io == t ) { \
		outport_##width( h+o, v ); \
	} else { \
		*(volatile type __attribute__((may_alias)) *)(h+o) = v; \
	}\
}
BUS_SPACE_DECL(u_int32_t, long, 4)
BUS_SPACE_DECL(u_int16_t, word, 2)
BUS_SPACE_DECL(u_int8_t,  byte, 1)

#elif defined(__PPC__)

#include <libcpu/io.h>

#if defined(_IO_BASE) && _IO_BASE == 0
#define BUS_SPACE_ALWAYS_MEM 1
#else
#define BUS_SPACE_ALWAYS_MEM 0
#endif

static inline void
bus_space_barrier(bus_space_tag_t t, bus_space_handle_t h, bus_size_t o, int width, int type)
{
	asm volatile("eieio");
}


#define BUS_SPACE_DECL(type, width, nwidth, op) \
static inline type \
bus_space_read_##nwidth(bus_space_tag_t t, bus_space_handle_t h, bus_size_t o) \
{ \
type v; \
	if ( !BUS_SPACE_ALWAYS_MEM && bus_space_io == t ) { \
		/* this is a macro setting the second argument */ \
		v = in_##op((volatile type *)(_IO_BASE+h+o)); \
	} else { \
		v = in_##op((volatile type *)(h+o)); \
	} \
	return v; \
} \
  \
static inline void \
bus_space_write_##nwidth(bus_space_tag_t t, bus_space_handle_t h, bus_size_t o, type v) \
{ \
	if ( !BUS_SPACE_ALWAYS_MEM && bus_space_io == t ) { \
		out_##op((volatile type *)(_IO_BASE+h+o), v); \
	} else { \
		out_##op((volatile type *)(h+o), v); \
	}\
}

BUS_SPACE_DECL(u_int32_t, long, 4, le32)
BUS_SPACE_DECL(u_int16_t, word, 2, le16)
BUS_SPACE_DECL(u_int8_t,  byte, 1, 8)

#undef BUS_SPACE_ALWAYS_MEM

#else
#error "Missing definitions of bus_space_XXX() for this CPU architecture"
#endif

#define bus_space_write_stream_4(_t, _h, _o, _v) \
  bus_space_write_4(_t, _h, _o, htole32(_v))

#undef BUS_SPACE_DECL

#ifndef BUS_PROBE_DEFAULT
#define BUS_PROBE_DEFAULT 0
#endif

/* error codes are > 0 ; low priority says that probe
 * was successful but another driver returning BUS_PROBE_DEFAULT
 * is to be preferred...
 */
#ifndef BUS_PROBE_LOW_PRIORITY
#define BUS_PROBE_LOW_PRIORITY (-1)
#endif



/* types -> -1 means unsupported */
#define SYS_RES_IOPORT  1
#define SYS_RES_MEMORY	2
#define SYS_RES_IRQ     3

/* flags (1<<31) means unsupported */
#define RF_ACTIVE       (1<<1)
#define RF_SHAREABLE    (1<<2)
#define RF_OPTIONAL     (1<<3)

struct resource *
bus_alloc_resource_any(device_t dev, int type, int *prid, unsigned flags);

int
bus_alloc_resources(device_t dev, struct resource_spec *rs, struct resource **res);

void
bus_release_resources(device_t dev, const struct resource_spec *rs, struct resource **res);

#define FILTER_STRAY 1
#define FILTER_HANDLED 0

typedef void (*driver_intr_t)(void *);
typedef int  (*driver_filter_t)(void *);

int
bus_setup_intr(device_t dev, struct resource *r, int flags, driver_filter_t filter, driver_intr_t handler, void *arg, void **cookiep);

/* Flags currently ignored... */
#define INTR_MPSAFE	    0
#define INTR_TYPE_NET   0

/*
 * INTR_FAST handlers are already more like 'filters',
 * i.e., they disable interrupts and schedule work
 * on a task queue.
 * 
 * During porting the fast handler has to be slightly
 * rewritten (must return an int value, FILTER_HANDLED
 * if a valid IRQ was detected and work has been scheduled
 * and FILTER_STRAY if this device didn't interrupt).
 *
 * You need to then remove INTR_FAST from the flags,
 * pass the converted handler as the 'filter' argument
 * and a NULL handler argument to bus_setup_intr().
 *
 */
extern int __INTR_FAST() __attribute__((
	error("\n\n==> you need to convert bus_setup_intr(INTR_FAST) to new API;\n"
              "    consult <sys/bus.h>\n\n")
));

/* Barf at compile time if they try to use INTR_FAST */
#define INTR_FAST       (__INTR_FAST())

int
bus_teardown_intr(device_t dev, struct resource *r, void *cookiep);

static inline int
bus_release_resource(device_t dev, int type, int rid, struct resource *r)
{
	return 0;
}

#define bus_generic_detach(dev) do {} while (0)

#define bus_generic_suspend(dev) (0)
#define bus_generic_resume(dev)  (0)

bus_space_handle_t
rman_get_bushandle(struct resource *r);

bus_space_tag_t
rman_get_bustag(struct resource *r);

/* Newer API (releng 7_1) */
static inline u_int8_t bus_read_1(struct resource *r, bus_size_t o)
{
	return bus_space_read_1(rman_get_bustag(r), rman_get_bushandle(r), o);
}

static inline u_int16_t bus_read_2(struct resource *r, bus_size_t o)
{
	return bus_space_read_2(rman_get_bustag(r), rman_get_bushandle(r), o);
}

static inline u_int32_t bus_read_4(struct resource *r, bus_size_t o)
{
	return bus_space_read_4(rman_get_bustag(r), rman_get_bushandle(r), o);
}

static inline void bus_write_1(struct resource *r, bus_size_t o, u_int8_t v)
{
	bus_space_write_1(rman_get_bustag(r), rman_get_bushandle(r), o, v);
}

static inline void bus_write_2(struct resource *r, bus_size_t o, u_int16_t v)
{
	bus_space_write_2(rman_get_bustag(r), rman_get_bushandle(r), o, v);
}

static inline void bus_write_4(struct resource *r, bus_size_t o, u_int32_t v)
{
	bus_space_write_4(rman_get_bustag(r), rman_get_bushandle(r), o, v);
}

#ifndef BUS_DMA_NOWAIT	
/* ignored anyways */
#define BUS_DMA_NOWAIT 0
#endif

#ifndef BUS_DMA_WAITOK	
/* ignored anyways */
#define BUS_DMA_WAITOK 0
#endif

#ifndef BUS_DMA_COHERENT	
/* ignored anyways */
#define BUS_DMA_COHERENT 0
#endif

#ifndef BUS_DMA_ZERO	
/* ignored anyways */
#define BUS_DMA_ZERO 0
#endif

#ifndef BUS_DMA_ALLOCNOW	
/* ignored anyways */
#define BUS_DMA_ALLOCNOW 0
#endif

#ifndef BUS_DMA_ZERO
#define BUS_DMA_ZERO 1
#endif

/* unused */
#ifndef BUS_SPACE_MAXADDR
#define BUS_SPACE_MAXADDR 0xdeadbeef
#endif

/* unused */
#ifndef BUS_SPACE_MAXADDR_32BIT
#define BUS_SPACE_MAXADDR_32BIT 0xdeadbeef
#endif

/* unused */
#ifndef BUS_SPACE_MAXSIZE_32BIT
#define BUS_SPACE_MAXSIZE_32BIT 0x10000000
#endif

typedef struct _bus_dma_tag_t {
	unsigned alignment;
	unsigned maxsize;
	unsigned maxsegs;
} * bus_dma_tag_t;

typedef struct _bus_dma_segment_t {
	bus_addr_t	ds_addr;
	bus_size_t  ds_len;
} bus_dma_segment_t;

typedef void *bus_dmamap_t;

int
bus_dma_tag_create(void *parent, unsigned alignment, unsigned bounds, uint32_t lowadd, uint32_t hiaddr, void (*filter)(void*), void *filterarg, unsigned maxsize, int nsegs, unsigned maxsegsize, unsigned flags, void (*lockfunc)(void*), void *lockarg, bus_dma_tag_t *ptag);

/* Dummy NULL fcn pointer */
#define busdma_lock_mutex 0

extern uint32_t __busdma_dummy_Giant;
#define Giant __busdma_dummy_Giant

void
bus_dma_tag_destroy(bus_dma_tag_t tag);

int
bus_dmamem_alloc(bus_dma_tag_t tag, void **p_vaddr, unsigned flags, bus_dmamap_t *p_map);

void
bus_dmamem_free(bus_dma_tag_t tag, void *vaddr, bus_dmamap_t map);
	
#ifndef CPU2BUSADDR
#ifndef PCI_DRAM_OFFSET
#define PCI_DRAM_OFFSET 0
#endif
#define CPU2BUSADDR(x) ((uint32_t)(x) + (PCI_DRAM_OFFSET))
#endif

#define kvtop(a)			CPU2BUSADDR((bus_addr_t)(a))
#define vtophys(a)			CPU2BUSADDR((bus_addr_t)(a))


static inline int
bus_dmamap_load_mbuf_sg(bus_dma_tag_t tag, bus_dmamap_t map, struct mbuf *m_head, bus_dma_segment_t *segs, int *pnsegs, unsigned flags)
{
struct mbuf *m;
int          n;
	for ( m=m_head, n=0; m; m=m->m_next, n++ ) {
		if ( n >= tag->maxsegs ) {
			return EFBIG;
		}
		segs[n].ds_addr = CPU2BUSADDR(mtod(m, unsigned));
		segs[n].ds_len  = m->m_len;
	}
	*pnsegs = n;
	return 0;
}

static inline bus_dma_tag_t
bus_get_dma_tag(device_t dev)
{
	return 0;
}

typedef void bus_dmamap_callback_t (void *arg, bus_dma_segment_t *segs, int nseg, int error);

static inline int
bus_dmamap_load(bus_dma_tag_t tag, bus_dmamap_t map, void *vaddr, bus_size_t size, bus_dmamap_callback_t cb, void *arg, unsigned flags)
{
bus_dma_segment_t segs[1];
	segs[0].ds_addr = CPU2BUSADDR(vaddr);
	segs[0].ds_len  = size;
	cb(arg, segs, 1, 0);
	return 0;
}

typedef void bus_dmamap_callback2_t (void *arg, bus_dma_segment_t *segs, int nsegs, bus_size_t mapsize, int error);

static inline int
bus_dmamap_load_mbuf(bus_dma_tag_t tag, bus_dmamap_t map, struct mbuf *m_head, bus_dmamap_callback2_t cb, void *arg, unsigned flags)
{
/* hopefully there's enough stack ... */
bus_dma_segment_t segs[tag->maxsegs];
struct mbuf *m;
int          n;
bus_size_t   sz;
	for ( m=m_head, sz=0, n=0; m; m=m->m_next, n++ ) {
		if ( n >= tag->maxsegs ) {
			cb(arg, segs, n, sz, EFBIG);
			return EFBIG;
		}
		segs[n].ds_addr = CPU2BUSADDR(mtod(m, unsigned));
		sz += (segs[n].ds_len  = m->m_len);
	}
	cb(arg, segs, n, sz, 0);
	return 0;
}

#define bus_dmamap_unload(tag, map) do {} while (0)

/* should we do something if we have no HW snooping ? */
#define bus_dmamap_sync(tag, map, flags) do { membarrier_rw(); } while (0)

#define bus_dmamap_create(tag, flags, pmap) ( *(pmap) = 0, 0 )
#define bus_dmamap_destroy(tag, map) do {} while (0)

int
resource_int_value(const char *name, int unit, const char *resname, int *result);
int
resource_long_value(const char *name, int unit, const char *resname, long *result);

#endif
