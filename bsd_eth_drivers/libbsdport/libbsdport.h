#ifndef RTEMS_COMPAT_DEFS_H
#define RTEMS_COMPAT_DEFS_H

#include <rtems.h>
#include <sys/param.h>

#ifndef _KERNEL
#define _KERNEL
#endif
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>

#include <inttypes.h>
#include <string.h>

#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/mbuf.h>

#include <rtems/bspIo.h>
#include <rtems/pci.h>
#include <rtems/irq.h>

#include <devicet.h>

#include <bsp/rtems_verscheck.h>

/*
#include <rtems/rtems_mii_ioctl.h>
*/



#include <rtems_udelay.h>

#ifndef bswap32
#define bswap32(_x) CPU_swap_u32(_x)
#endif

#if defined(__LITTLE_ENDIAN__) || defined(__i386__)
static inline uint16_t  htole16(uint16_t  v) { return v; }
static inline uint32_t  htole32(uint32_t  v) { return v; }
static inline uint64_t  htole64(uint64_t  v) { return v; }
static inline uint16_t  le16toh(uint16_t  v) { return v; }
static inline uint32_t  le32toh(uint32_t  v) { return v; }
static inline uint64_t  le64toh(uint64_t  v) { return v; }

#ifdef __i386__

#ifdef __SSE__
static inline void membarrier_r()  { asm volatile("lfence":::"memory"); }
static inline void membarrier_rw() { asm volatile("mfence":::"memory"); }
/* Current x86 CPUs always do in-order stores - prevent the compiler from reordering, neverthelesss */
static inline void membarrier_w()  { asm volatile(/*"sfence"*/"":::"memory"); }
#else
static inline void membarrier_r()  { asm volatile("lock; addl $0,0(%%esp)":::"memory"); }
static inline void membarrier_rw() { asm volatile("lock; addl $0,0(%%esp)":::"memory"); }
/* Current x86 CPUs always do in-order stores - prevent the compiler from reordering, neverthelesss */
static inline void membarrier_w()  { asm volatile(/*"lock; addl $0,0(%%esp)"*/"":::"memory"); }
#endif

#endif

#elif defined(__BIG_ENDIAN__)
#ifdef __PPC__
#include <libcpu/byteorder.h>

/* Note the 'may_alias' constructs. They are
 * a safeguard agains the alias rule should the
 * pointer argument of st_leXX change (again) in
 * the future (and it should be safe to use older
 * versions of 'byteorder.h'
 */

static inline uint16_t
htole16(uint16_t v)
{
uint16_t rval __attribute__((may_alias));
	st_le16((volatile uint16_t*)&rval,v);
	return rval;
}

static inline uint16_t
le16toh(uint16_t v)
{
uint16_t vv __attribute__((may_alias)) = v;
	return ld_le16((volatile uint16_t*)&vv);
}

static inline uint32_t
htole32(uint32_t v)
{
uint32_t rval __attribute__((may_alias));
	st_le32((volatile libbsdport_u32_t*)&rval,v);
	return rval;
}

static inline uint32_t
le32toh(uint32_t v)
{
uint32_t vv __attribute__((may_alias)) = v;
	return ld_le32((volatile libbsdport_u32_t*)&vv);
}

/* Compiler generated floating point instructions for this
 * and rtems_bsdnet_newproc()-generated tasks are non-FP
 * :-(
 */
static inline uint64_t
htole64(uint64_t  v) 
{
union {
	libbsdport_u32_t tmp[2] __attribute__((may_alias));
	uint64_t rval   __attribute__((may_alias));
} u;

	st_le32( &u.tmp[0], (unsigned)(v&0xffffffff) );
	st_le32( &u.tmp[1], (unsigned)((v>>32)&0xffffffff) );

	return u.rval;
}

static inline void membarrier_r()  { asm volatile("sync":::"memory"); }

static inline void membarrier_rw() { asm volatile("sync":::"memory"); }

static inline void membarrier_w()  { asm volatile("eieio":::"memory"); }

#else
#error "need htoleXX() implementation for this CPU arch"
#endif

#else
#error "Unknown CPU endianness"
#endif

static __inline void
le32enc(void *pp, uint32_t u)
{
  unsigned char *p = (unsigned char *)pp;

  p[0] = u & 0xff;
  p[1] = (u >> 8) & 0xff;
  p[2] = (u >> 16) & 0xff;
  p[3] = (u >> 24) & 0xff;
}

#include <mutex.h>
#include <callout.h>

#ifndef PCIR_BAR
#define PCIR_BAR(x) (0x10+4*(x))
#endif

#ifndef PCIR_COMMAND
#define PCIR_COMMAND		PCI_COMMAND
#endif

#ifndef PCIR_REVID
#define PCIR_REVID			PCI_REVISION_ID
#endif

#ifndef PCIR_SUBVEND_0
#define PCIR_SUBVEND_0		PCI_SUBSYSTEM_VENDOR_ID
#endif

#ifndef PCIR_SUBDEV_0
#define PCIR_SUBDEV_0		PCI_SUBSYSTEM_ID
#endif

#ifndef PCIR_CIS
#define PCIR_CIS			PCI_CARDBUS_CIS
#endif

#ifndef PCIM_CMD_BUSMASTEREN
#define PCIM_CMD_BUSMASTEREN PCI_COMMAND_MASTER
#endif

#ifndef PCIM_CMD_MEMEN
#define PCIM_CMD_MEMEN		PCI_COMMAND_MEMORY
#endif

#ifndef PCIM_CMD_PORTEN
#define PCIM_CMD_PORTEN		PCI_COMMAND_IO
#endif

#ifndef PCIR_CAP_PTR
#define PCIR_CAP_PTR 0x34
#endif

#ifndef PCIR_POWER_STATUS
#define PCIR_POWER_STATUS 0x4
#endif

#ifndef PCIR_CACHELNSZ
#define PCIR_CACHELNSZ PCI_CACHE_LINE_SIZE
#endif

#ifndef PCIM_PSTAT_PME
#define PCIM_PSTAT_PME       0x8000
#endif

#ifndef PCIM_PSTAT_PMEENABLE
#define PCIM_PSTAT_PMEENABLE 0x0100
#endif

#ifndef PCIM_CMD_MWRICEN
#define PCIM_CMD_MWRICEN PCI_COMMAND_INVALIDATE
#endif

#ifndef PCIY_PMG
#define PCIY_PMG             0x01
#endif

#ifndef PCI_RF_DENSE
#define PCI_RF_DENSE         0
#endif 

static inline uint32_t
pci_read_config(device_t dev, unsigned reg, int width)
{
	switch ( width ) {
		default:
		case 4:
			{
				libbsdport_u32_t v;
				pci_read_config_dword(dev->bushdr.pci.bus, dev->bushdr.pci.dev, dev->bushdr.pci.fun, reg, &v);
				return v;
			}
		case 2:
			{
				uint16_t v;
				pci_read_config_word(dev->bushdr.pci.bus, dev->bushdr.pci.dev, dev->bushdr.pci.fun, reg, &v);
				return (uint32_t)v;
			}	
		case 1:
			{
				uint8_t v;
				pci_read_config_byte(dev->bushdr.pci.bus, dev->bushdr.pci.dev, dev->bushdr.pci.fun, reg, &v);
				return (uint32_t)v;
			}
	}
}

static inline void
pci_write_config(device_t dev, unsigned reg, uint32_t val, int width)
{
	switch ( width ) {
		default:
		case 4:
			{
				pci_write_config_dword(dev->bushdr.pci.bus, dev->bushdr.pci.dev, dev->bushdr.pci.fun, reg, val);
			}
		case 2:
			{
				pci_write_config_word(dev->bushdr.pci.bus, dev->bushdr.pci.dev, dev->bushdr.pci.fun, reg, val);
			}	
		case 1:
			{
				pci_write_config_byte(dev->bushdr.pci.bus, dev->bushdr.pci.dev, dev->bushdr.pci.fun, reg, val);
			}
	}
}


static inline uint16_t
pci_get_vendor(device_t dev)
{
	return pci_read_config(dev, PCI_VENDOR_ID, 2);
}

static inline uint16_t
pci_get_device(device_t dev)
{
	return pci_read_config(dev, PCI_DEVICE_ID, 2);
}

static inline uint16_t
pci_get_subvendor(device_t dev)
{
	return pci_read_config(dev, PCIR_SUBVEND_0, 2);
}

static inline uint16_t
pci_get_subdevice(device_t dev)
{
	return pci_read_config(dev, PCIR_SUBDEV_0, 2);
}

static inline uint8_t
pci_get_revid(device_t dev)
{
  return pci_read_config(dev, PCIR_REVID, 1);
}

static inline void
pci_enable_busmaster(device_t dev)
{
	pci_write_config(
		dev,
		PCI_COMMAND,
		pci_read_config(dev, PCI_COMMAND, 2) | PCI_COMMAND_MASTER,
		2
	);
}

static inline void
pci_enable_io(device_t dev, int space)
{
	pci_write_config(
		dev,
		PCI_COMMAND,
		pci_read_config(dev, PCI_COMMAND, 2) | space,
		2
	);
}

static inline void
pci_disable_io(device_t dev, int space)
{
	pci_write_config(
		dev,
		PCI_COMMAND,
		pci_read_config(dev, PCI_COMMAND, 2) & ~space,
		2
	);
}



/* MSI / MSIX not supported */
static inline int
pci_msi_count(device_t dev) { return 0; }

static inline int
pci_alloc_msi(device_t dev, int *pval) { return -1; }

static inline int
pci_alloc_msix(device_t dev, int *pval) { return -1; }

static inline void
pci_release_msi(device_t dev) { }




#define IFQ_DRV_IS_EMPTY(q) (0 == (q)->ifq_head)
#define IFQ_DRV_DEQUEUE(q,m) IF_DEQUEUE((q),(m))
#define IFQ_DRV_PREPEND(q,m) IF_PREPEND((q),(m))

#define ifq_drv_maxlen ifq_maxlen
#define IFQ_SET_MAXLEN(q, len) do {} while (0)
#define IFQ_SET_READY(q)       do {} while (0)

#define ETHER_BPF_MTAP(ifp, m) do {} while (0)
#define BPF_MTAP(ifp, m)       do {} while (0)

#define IF_LLADDR(ifp)	(((struct arpcom *)(ifp))->ac_enaddr)

#define if_link_state_change(ifp, state) do {} while (0)

#define if_maddr_rlock(ifp) do {} while (0)
#define if_maddr_runlock(ifp) do {} while (0)

/* if_name should probably be const char * but isn't */
#define if_initname(ifp, name, unit) \
	do { (ifp)->if_name = (char*)(name); (ifp)->if_unit = (unit); } while (0)

struct ifnet * if_alloc(int type);

void if_free(struct ifnet *ifp);

#define if_printf(ifp,args...)  do { printf("%s: ",(ifp)->if_name); printf(args); } while (0)

void *
contigmalloc(
	unsigned long size,
	int type,
	int flags,
	unsigned long lo,
	unsigned long hi,
	unsigned long align,
	unsigned long bound);

void
contigfree(void *ptr, size_t size, int type);

/* locking is handled by 'super-lock' outside driver; watch for link intr task, though */
#define NET_LOCK_GIANT()	do {} while (0)
#define NET_UNLOCK_GIANT()	do {} while (0)

#define KASSERT(cond, msg...)	\
	do { \
	if ( ! (cond) ) { \
		rtems_panic msg;  \
	} \
	} while (0)

#define __FBSDID(x)
#define MODULE_DEPEND(x1,x2,x3,x4,x5)

void *
real_libc_malloc(size_t);

void
real_libc_free(void*);

extern int libbsdport_bootverbose;
/* Try not to pollute global namespace */
#define bootverbose libbsdport_bootverbose

#endif
