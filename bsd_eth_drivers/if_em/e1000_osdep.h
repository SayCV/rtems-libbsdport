/**************************************************************************

Copyright (c) 2001-2007, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 3. Neither the name of the Intel Corporation nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/
/*$FreeBSD: src/sys/dev/em/e1000_osdep.h,v 1.3 2007/05/16 00:14:23 jfv Exp $*/


#ifndef _FREEBSD_OS_H_
#define _FREEBSD_OS_H_

#include <rtems.h>
#include <bsp.h>
#include <rtems/pci.h>
#include <vm/vm.h> /* for non-_KERNEL boolean_t :-( */

/* The happy-fun DELAY macro is defined in /usr/src/sys/i386/include/clock.h */
#define usec_delay(x) e1000_udelay(x)
#define msec_delay(x) e1000_udelay(1000*(x))
/* TODO: Should we be paranoid about delaying in interrupt context? */
#define msec_delay_irq(x) msec_delay(x)

void e1000_udelay(unsigned);

#define MSGOUT(S, A, B)     printf(S "\n", A, B)
#define DEBUGFUNC(F)        DEBUGOUT(F);
	#define DEBUGOUT(S)
	#define DEBUGOUT1(S,A)
	#define DEBUGOUT2(S,A,B)
	#define DEBUGOUT3(S,A,B,C)
	#define DEBUGOUT7(S,A,B,C,D,E,F,G)


#define STATIC				static

#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif

#define CMD_MEM_WRT_INVALIDATE          0x0010  /* BIT_4 */
#define PCI_COMMAND_REGISTER            PCIR_COMMAND

/*
** These typedefs are necessary due to the new
** shared code, they are native to Linux.
*/
typedef uint64_t	u64;
typedef uint32_t	u32;
typedef uint16_t	u16;
typedef uint8_t		u8 ;
typedef int64_t		s64;
typedef int32_t		s32;
typedef int16_t		s16;
typedef int8_t		s8 ;

typedef volatile uint32_t __uint32_va_t __attribute__((may_alias));
typedef volatile uint16_t __uint16_va_t __attribute__((may_alias));

struct e1000_pcisig {
	uint32_t sig;
};

/* Register an instance; if this returns nonzero
 * then registration failed and the device with
 * the pci signature passed in MUST NOT be used
 * (since it is already in use by another driver).
 */
int
e1000_register(struct e1000_pcisig *sig_p_out, unsigned bus, unsigned dev, unsigned fun);

void
e1000_unregister(struct e1000_pcisig *sig_p);

#ifdef NO_82542_SUPPORT
#define E1000_REGISTER(hw, reg) reg
#else
#define E1000_REGISTER(hw, reg) (((hw)->mac.type >= e1000_82543) \
    ? reg : e1000_translate_register_82542(reg))
#endif

#define E1000_WRITE_FLUSH(a) E1000_READ_REG(a, E1000_STATUS)

/* Provide our own I/O so that the low-level driver API can
 * be used independently from the BSD stuff.
 * This is useful for people who want to use an e1000 adapter
 * for special ethernet links that do not use BSD TCP/IP.
 */
#ifdef __PPC__

#include <libcpu/io.h>

static inline uint16_t __in_le16(uint8_t *base, uint32_t offset)
{
uint16_t     rval;
    __asm__ __volatile__(
        "lhbrx %0,%2,%1; eieio\n"
            : "=r" (rval)
            : "r"(base), "b"(offset), "m"(*(__uint16_va_t*)(base + offset))
    );
    return rval;
}

static inline void __out_le16(uint8_t *base, uint32_t offset, uint16_t val)
{
    __asm__ __volatile__(
        "sthbrx %1,%3,%2; eieio"
            : "=o"(*(__uint16_va_t*)(base+offset))
            : "r"(val), "r"(base), "b"(offset)
    );
}

static inline uint32_t __in_le32(uint8_t *base, uint32_t offset)
{
uint32_t     rval;
    __asm__ __volatile__(
        "lwbrx %0,%2,%1; eieio\n"
            : "=r" (rval)
            : "r"(base), "b"(offset), "m"(*(__uint32_va_t*)(base + offset))
    );
    return rval;
}

static inline void __out_le32(uint8_t *base, uint32_t offset, uint32_t val)
{
    __asm__ __volatile__(
        "stwbrx %1,%3,%2; eieio"
            : "=o"(*(__uint32_va_t*)(base+offset))
            : "r"(val), "r"(base), "b"(offset)
    );
}

#ifdef _IO_BASE
static inline void __outport_dword(unsigned long base, uint32_t off, uint32_t val)
{
	__out_le32((uint8_t*)(_IO_BASE+base), off, val);
}
#else
#error "_IO_BASE needs to be defined by BSP (bsp.h)"
#endif

#elif defined(__i386__)
#include <libcpu/cpu.h>

static inline uint16_t __in_le16(uint8_t *base, uint32_t offset)
{
	return *(__uint16_va_t*)(base + offset);
}

static inline void __out_le16(uint8_t *base, uint32_t offset, uint16_t val)
{
	*(__uint16_va_t*)(base + offset) = val;
}

static inline uint32_t __in_le32(uint8_t *base, uint32_t offset)
{
	return *(__uint32_va_t*)(base + offset);
}

static inline void __out_le32(uint8_t *base, uint32_t offset, uint32_t val)
{
	*(__uint32_va_t*)(base + offset) = val;
}

static inline void __outport_dword(unsigned long base, uint32_t off, uint32_t val)
{
	i386_outport_long( (base + off), val );
}

#else
#warning "not ported to this CPU architecture yet -- using libbsdport I/O"
#define  USE_LIBBSDPORT_IO
#endif

#if defined(USE_LIBBSDPORT_IO) && !defined(_KERNEL)
#define _KERNEL
#endif

#ifdef   _KERNEL
#ifndef __INSIDE_RTEMS_BSD_TCPIP_STACK__
#define __INSIDE_RTEMS_BSD_TCPIP_STACK__
#endif
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/bus.h>

#define ASSERT(x) if(!(x)) panic("EM: x")

#include <devicet.h>

struct e1000_osdep
{
	/* struct e1000_pcisig MUST be first since
	 * 'back' pointer is cast to (struct e1000_pcisig *)
	 * in e1000_osdep.c!
	 */
	struct e1000_pcisig pcisig;
	uint32_t mem_bus_space_handle;
	uint32_t io_bus_space_handle;
	uint32_t flash_bus_space_handle;
	/* these are currently unused; present for freebsd compatibility only */
	uint32_t mem_bus_space_tag;
	uint32_t io_bus_space_tag;
	uint32_t flash_bus_space_tag;
	device_t dev;	
};
#endif

#ifdef USE_LIBBSDPORT_IO

#define USE_EXPLICIT_BUSTAGS

#ifdef USE_EXPLICIT_BUSTAGS /* Help compiler by specifying explicit bus tags */

/* Read from an absolute offset in the adapter's memory space */
#define E1000_READ_OFFSET(hw, offset) \
    bus_space_read_4(bus_space_mem, \
    ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, offset)

/* Write to an absolute offset in the adapter's memory space */
#define E1000_WRITE_OFFSET(hw, offset, value) \
    bus_space_write_4(bus_space_mem, \
    ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, offset, value)

/* Register READ/WRITE macros */

#define E1000_READ_REG(hw, reg) \
    bus_space_read_4(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg))

#define E1000_WRITE_REG(hw, reg, value) \
    bus_space_write_4(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg), value)

#define E1000_READ_REG_ARRAY(hw, reg, index) \
    bus_space_read_4(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + ((index)<< 2))

#define E1000_WRITE_REG_ARRAY(hw, reg, index, value) \
    bus_space_write_4(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + ((index)<< 2), value)

#define E1000_READ_REG_ARRAY_DWORD E1000_READ_REG_ARRAY
#define E1000_WRITE_REG_ARRAY_DWORD E1000_WRITE_REG_ARRAY

#define E1000_READ_REG_ARRAY_BYTE(hw, reg, index) \
    bus_space_read_1(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + index)

#define E1000_WRITE_REG_ARRAY_BYTE(hw, reg, index, value) \
    bus_space_write_1(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + index, value)

#define E1000_WRITE_REG_ARRAY_WORD(hw, reg, index, value) \
    bus_space_write_2(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + (index << 1), value)

#define E1000_WRITE_REG_IO(hw, reg, value) do {\
    bus_space_write_4(bus_space_io, \
        ((struct e1000_osdep *)(hw)->back)->io_bus_space_handle, \
        (hw)->io_base, reg); \
    bus_space_write_4(bus_space_io, \
        ((struct e1000_osdep *)(hw)->back)->io_bus_space_handle, \
        (hw)->io_base + 4, value); } while (0)

#define E1000_READ_FLASH_REG(hw, reg) \
    bus_space_read_4(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->flash_bus_space_handle, reg)

#define E1000_READ_FLASH_REG16(hw, reg) \
    bus_space_read_2(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->flash_bus_space_handle, reg)

#define E1000_WRITE_FLASH_REG(hw, reg, value) \
    bus_space_write_4(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->flash_bus_space_handle, reg, value)

#define E1000_WRITE_FLASH_REG16(hw, reg, value) \
    bus_space_write_2(bus_space_mem, \
        ((struct e1000_osdep *)(hw)->back)->flash_bus_space_handle, reg, value)

#else /* USE_EXPLICIT_BUSTAGS */

/* Read from an absolute offset in the adapter's memory space */
#define E1000_READ_OFFSET(hw, offset) \
    bus_space_read_4(((struct e1000_osdep *)(hw)->back)->mem_bus_space_tag, \
    ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, offset)

/* Write to an absolute offset in the adapter's memory space */
#define E1000_WRITE_OFFSET(hw, offset, value) \
    bus_space_write_4(((struct e1000_osdep *)(hw)->back)->mem_bus_space_tag, \
    ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, offset, value)

/* Register READ/WRITE macros */

#define E1000_READ_REG(hw, reg) \
    bus_space_read_4(((struct e1000_osdep *)(hw)->back)->mem_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg))

#define E1000_WRITE_REG(hw, reg, value) \
    bus_space_write_4(((struct e1000_osdep *)(hw)->back)->mem_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg), value)

#define E1000_READ_REG_ARRAY(hw, reg, index) \
    bus_space_read_4(((struct e1000_osdep *)(hw)->back)->mem_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + ((index)<< 2))

#define E1000_WRITE_REG_ARRAY(hw, reg, index, value) \
    bus_space_write_4(((struct e1000_osdep *)(hw)->back)->mem_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + ((index)<< 2), value)

#define E1000_READ_REG_ARRAY_DWORD E1000_READ_REG_ARRAY
#define E1000_WRITE_REG_ARRAY_DWORD E1000_WRITE_REG_ARRAY

#define E1000_READ_REG_ARRAY_BYTE(hw, reg, index) \
    bus_space_read_1(((struct e1000_osdep *)(hw)->back)->mem_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + index)

#define E1000_WRITE_REG_ARRAY_BYTE(hw, reg, index, value) \
    bus_space_write_1(((struct e1000_osdep *)(hw)->back)->mem_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + index, value)

#define E1000_WRITE_REG_ARRAY_WORD(hw, reg, index, value) \
    bus_space_write_2(((struct e1000_osdep *)(hw)->back)->mem_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->mem_bus_space_handle, \
        E1000_REGISTER(hw, reg) + (index << 1), value)

#define E1000_WRITE_REG_IO(hw, reg, value) do {\
    bus_space_write_4(((struct e1000_osdep *)(hw)->back)->io_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->io_bus_space_handle, \
        (hw)->io_base, reg); \
    bus_space_write_4(((struct e1000_osdep *)(hw)->back)->io_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->io_bus_space_handle, \
        (hw)->io_base + 4, value); } while (0)

#define E1000_READ_FLASH_REG(hw, reg) \
    bus_space_read_4(((struct e1000_osdep *)(hw)->back)->flash_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->flash_bus_space_handle, reg)

#define E1000_READ_FLASH_REG16(hw, reg) \
    bus_space_read_2(((struct e1000_osdep *)(hw)->back)->flash_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->flash_bus_space_handle, reg)

#define E1000_WRITE_FLASH_REG(hw, reg, value) \
    bus_space_write_4(((struct e1000_osdep *)(hw)->back)->flash_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->flash_bus_space_handle, reg, value)

#define E1000_WRITE_FLASH_REG16(hw, reg, value) \
    bus_space_write_2(((struct e1000_osdep *)(hw)->back)->flash_bus_space_tag, \
        ((struct e1000_osdep *)(hw)->back)->flash_bus_space_handle, reg, value)
#endif /* USE_EXPLICIT_BUSTAGS */

#else /* USE_LIBBSDPORT_IO */

/* Read from an absolute offset in the adapter's memory space */
#define E1000_READ_OFFSET(hw, offset) \
	__in_le32((hw)->hw_addr, offset)

/* Write to an absolute offset in the adapter's memory space */
#define E1000_WRITE_OFFSET(hw, offset, value) \
	__out_le32((hw)->hw_addr, offset, value)

/* Register READ/WRITE macros */

#define E1000_READ_REG(hw, reg) \
	__in_le32((hw)->hw_addr, E1000_REGISTER(hw, reg))

#define E1000_WRITE_REG(hw, reg, value) \
	__out_le32((hw)->hw_addr, E1000_REGISTER(hw, reg), value)

#define E1000_READ_REG_ARRAY(hw, reg, index) \
	__in_le32((hw)->hw_addr, E1000_REGISTER(hw, reg) + ((index)<< 2))

#define E1000_WRITE_REG_ARRAY(hw, reg, index, value) \
	__out_le32((hw)->hw_addr, E1000_REGISTER(hw, reg) + ((index)<< 2), value)

#define E1000_READ_REG_ARRAY_DWORD E1000_READ_REG_ARRAY
#define E1000_WRITE_REG_ARRAY_DWORD E1000_WRITE_REG_ARRAY

#define E1000_WRITE_REG_IO(hw, reg, value) do { \
	__outport_dword((hw)->io_base, 0, reg);     \
	__outport_dword((hw)->io_base, 4, value);   \
	} while (0)

#define E1000_READ_FLASH_REG(hw, reg) \
	__in_le32( (hw)->flash_address, reg )

#define E1000_READ_FLASH_REG16(hw, reg) \
	__in_le16( (hw)->flash_address, reg )

#define E1000_WRITE_FLASH_REG(hw, reg, value) \
	__out_le32( (hw)->flash_address, reg, value )

#define E1000_WRITE_FLASH_REG16(hw, reg, value) \
	__out_le16( (hw)->flash_address, reg, value )

#endif /* USE_LIBBSDPORT_IO */


#endif  /* _FREEBSD_OS_H_ */

