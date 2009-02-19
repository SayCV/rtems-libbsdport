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
#define _KERNEL
#include <rtems/rtems_bsdnet_internal.h>
#include <bsp.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <rtems/pci.h>

#define ASSERT(x) if(!(x)) panic("EM: x")

/* The happy-fun DELAY macro is defined in /usr/src/sys/i386/include/clock.h */
#define usec_delay(x) DELAY(x)
#define msec_delay(x) DELAY(1000*(x))
/* TODO: Should we be paranoid about delaying in interrupt context? */
#define msec_delay_irq(x) DELAY(1000*(x))
#include <rtems_udelay.h>

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

#include <devicet.h>

struct e1000_osdep
{
	uint32_t mem_bus_space_handle;
	uint32_t io_bus_space_handle;
	uint32_t flash_bus_space_handle;
	/* these are currently unused; present for freebsd compatibility only */
	uint32_t mem_bus_space_tag;
	uint32_t io_bus_space_tag;
	uint32_t flash_bus_space_tag;
	device_t dev;	
};

typedef volatile uint32_t __attribute__((may_alias)) *__uint32_a_p_t;
typedef volatile uint16_t __attribute__((may_alias)) *__uint16_a_p_t;
typedef volatile uint8_t  __attribute__((may_alias)) * __uint8_a_p_t;

#ifdef __PPC__
#include <libcpu/io.h>
static inline uint8_t __in_8(uint32_t base, uint32_t offset)
{
__uint8_a_p_t a = (__uint8_a_p_t)(base+offset);
uint8_t     rval;
	__asm__ __volatile__(
		"sync;\n"
		"lbz%U1%X1 %0,%1;\n"
		"twi 0,%0,0;\n"
		"isync" : "=r" (rval) : "m"(*a));
	return rval;
}

static inline void __out_8(uint32_t base, uint32_t offset, uint8_t val)
{
__uint8_a_p_t a = (__uint8_a_p_t)(base+offset);
	__asm__ __volatile__(
		"stb%U0%X0 %1,%0; eieio" : "=m" (*a) : "r"(val)
	);
}

static inline uint16_t __in_le16(uint32_t base, uint32_t offset)
{
__uint16_a_p_t a = (__uint16_a_p_t)(base+offset);
uint16_t     rval;
	__asm__ __volatile__(
		"sync;\n"
		"lhbrx %0,0,%1;\n"
		"twi 0,%0,0;\n"
		"isync" : "=r" (rval) : "r"(a), "m"(*a));
	return rval;
}

static inline void __out_le16(uint32_t base, uint32_t offset, uint16_t val)
{
__uint16_a_p_t a = (__uint16_a_p_t)(base+offset);
	__asm__ __volatile__(
		"sync; sthbrx %1,0,%2" : "=m" (*a) : "r"(val), "r"(a)
	);
}

static inline uint32_t __in_le32(uint32_t base, uint32_t offset)
{
__uint32_a_p_t a = (__uint32_a_p_t)(base+offset);
uint32_t     rval;
	__asm__ __volatile__(
		"sync;\n"
		"lwbrx %0,0,%1;\n"
		"twi 0,%0,0;\n"
		"isync" : "=r" (rval) : "r"(a), "m"(*a));
	return rval;
}

static inline void __out_le32(uint32_t base, uint32_t offset, uint32_t val)
{
__uint32_a_p_t a = (__uint32_a_p_t)(base+offset);
	__asm__ __volatile__(
		"sync; stwbrx %1,0,%2" : "=m" (*a) : "r"(val), "r"(a)
	);
}

#ifdef _IO_BASE
static inline void __outport_dword(uint32_t base, uint32_t off, uint32_t val)
{
	__out_le32(_IO_BASE+base+off,0,val);
}
#else
#error "_IO_BASE needs to be defined by BSP (bsp.h)"
#endif

#elif defined(__i386__)
#include <libcpu/cpu.h>
static inline uint8_t __in_8(uint32_t base, uint32_t offset)
{
__uint8_a_p_t a = (__uint8_a_p_t)(base+offset);
	return *a;
}

static inline void __out_8(uint32_t base, uint32_t offset, uint8_t val)
{
__uint8_a_p_t a = (__uint8_a_p_t)(base+offset);
	*a = val;
}

static inline uint16_t __in_le16(uint32_t base, uint32_t offset)
{
__uint16_a_p_t a = (__uint16_a_p_t)(base+offset);
	return *a;
}

static inline void __out_le16(uint32_t base, uint32_t offset, uint16_t val)
{
__uint16_a_p_t a = (__uint16_a_p_t)(base+offset);
	*a = val;
}

static inline uint32_t __in_le32(uint32_t base, uint32_t offset)
{
__uint32_a_p_t a = (__uint32_a_p_t)(base+offset);
	return *a;
}

static inline void __out_le32(uint32_t base, uint32_t offset, uint32_t val)
{
__uint32_a_p_t a = (__uint32_a_p_t)(base+offset);
	*a = val;
}


static inline void __outport_dword(uint32_t base, uint32_t off, uint32_t val)
{
	i386_outport_long( (base + off), val );
}

#else
#error "not ported to this CPU architecture yet"
#endif

#ifdef NO_82542_SUPPORT
#define E1000_REGISTER(hw, reg) reg
#else
#define E1000_REGISTER(hw, reg) (((hw)->mac.type >= e1000_82543) \
    ? reg : e1000_translate_register_82542(reg))
#endif

#define E1000_WRITE_FLUSH(a) E1000_READ_REG(a, E1000_STATUS)

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

#endif  /* _FREEBSD_OS_H_ */

