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

/*
 * NOTE: the following routines using the e1000 
 * 	naming style are provided to the shared
 *	code which expects that rather than 'em'
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/pci.h>
#include <e1000_api.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#ifndef  PCIR_COMMAND
#define  PCIR_COMMAND PCI_COMMAND
#endif

#define PCISIG_INVAL 0xffffffff

#define PCISIG_MK(b,d,f) ( ((b)<<8) | (((d)&0x1f)<<3) | ((f)&7) )

#define PCISIG_BUS(sig)  ( ((sig) >> 8) & 0xffffff )
#define PCISIG_DEV(sig)  ( ((sig) >> 3) & 0x1f     )
#define PCISIG_FUN(sig)  (  (sig)       & 0x07     )


static uint32_t e1k_devs[]={
	PCISIG_INVAL,
	PCISIG_INVAL,
	PCISIG_INVAL,
	PCISIG_INVAL,
	PCISIG_INVAL,
	PCISIG_INVAL,
	PCISIG_INVAL,
	PCISIG_INVAL
};

#define N_E1K_DEVS (sizeof(e1k_devs)/sizeof(e1k_devs[0]))

void
e1000_write_pci_cfg(struct e1000_hw *hw, uint32_t reg, uint16_t *value)
{
struct e1000_pcisig *s_p = hw->back;
uint32_t             s   = s_p->sig;

	pci_write_config_word( PCISIG_BUS(s), PCISIG_DEV(s), PCISIG_FUN(s), reg, *value );
}

void
e1000_read_pci_cfg(struct e1000_hw *hw, uint32_t reg, uint16_t *value)
{
struct e1000_pcisig *s_p = hw->back;
uint32_t             s   = s_p->sig;
	pci_read_config_word( PCISIG_BUS(s), PCISIG_DEV(s), PCISIG_FUN(s), reg, value );
}

void
e1000_pci_set_mwi(struct e1000_hw *hw)
{
uint16_t v = (hw->bus.pci_cmd_word | CMD_MEM_WRT_INVALIDATE);
	e1000_write_pci_cfg( hw, PCIR_COMMAND, &v );
}

void
e1000_pci_clear_mwi(struct e1000_hw *hw)
{
uint16_t v = (hw->bus.pci_cmd_word & ~CMD_MEM_WRT_INVALIDATE);
	e1000_write_pci_cfg( hw, PCIR_COMMAND, &v );
}

/*
 * Read the PCI Express capabilities
 */
int32_t
e1000_read_pcie_cap_reg(struct e1000_hw *hw, uint32_t reg, uint16_t *value)
{
	int32_t		error = E1000_SUCCESS;
	uint16_t	cap_off;

	switch (hw->mac.type) {

		case e1000_82571:
		case e1000_82572:
		case e1000_82573:
		case e1000_80003es2lan:
			cap_off = 0xE0;
			e1000_read_pci_cfg(hw, cap_off + reg, value);
			break;
		default:
			error = ~E1000_NOT_IMPLEMENTED;
			break;
	}

	return (error);	
}

int32_t
e1000_alloc_zeroed_dev_spec_struct(struct e1000_hw *hw, uint32_t size)
{
	return (hw->dev_spec = calloc(1, size)) ? 0 : ENOMEM;
}

void
e1000_free_dev_spec_struct(struct e1000_hw *hw)
{
	free ( hw->dev_spec );
	hw->dev_spec = 0;
}


void
e1000_udelay(unsigned usecs)
{
rtems_interval  clock_f, ticks;
uint64_t        tmp;
struct timespec then, now; 
	rtems_clock_get( RTEMS_CLOCK_GET_TICKS_PER_SECOND, &clock_f );

	/* round up to next tick: 
		floor( f*T + .5 ) = floor( f*T_us/1e6 + 0.5 )
                          = floor( (f * T_us + 5e5) / 1e6 )
	 */
	tmp  = (uint64_t)clock_f * (uint64_t)usecs;

	if ( tmp < 500000ULL ) {
		/* less than half a tick -- busy wait */
		clock_gettime( CLOCK_REALTIME, &then );
		then.tv_sec  += usecs/1000000;
		then.tv_nsec += (usecs % 1000000)*1000;
		if ( then.tv_nsec >= 1000000000 ) {
			then.tv_nsec -= 1000000000;
			then.tv_sec++;
		}

		do {
			clock_gettime( CLOCK_REALTIME, &now );

		} while (  now.tv_sec < then.tv_sec ||
		          (now.tv_sec == then.tv_sec && now.tv_nsec < then.tv_nsec) );
	} else {
		tmp += 500000ULL;
		tmp /= 1000000ULL;

		ticks = (rtems_interval)tmp;

		rtems_task_wake_after( ticks );
	}
}

int
e1000_register(struct e1000_pcisig *s_p, unsigned b, unsigned d, unsigned f )
{
int      i,j,key;
uint32_t sig = PCISIG_MK(b,d,f);

	if ( PCISIG_INVAL == sig )
		return -1;

	j = -1;

	rtems_interrupt_disable(key);
	for ( i = 0; i<N_E1K_DEVS; i++ ) {
		if ( PCISIG_INVAL == e1k_devs[i] ) {
			j = i;
		} else if ( sig == e1k_devs[i] ) {
			j = -1;
			break;
		}
	}
	if ( j >= 0 ) {
		e1k_devs[j] = sig;
		if ( s_p )
			s_p->sig = sig;
	}
	rtems_interrupt_enable(key);

	return j < 0;
}

void
e1000_unregister(struct e1000_pcisig *s_p)
{
int      i,key;
uint32_t sig = s_p ? s_p->sig : PCISIG_INVAL;

	if ( PCISIG_INVAL == sig )
		return;

	rtems_interrupt_disable(key);
	for ( i = 0; i<N_E1K_DEVS; i++ ) {
		if ( sig == e1k_devs[i] ) {
			e1k_devs[i] = PCISIG_INVAL;
			if ( s_p ) 
				s_p->sig = PCISIG_INVAL;
			break;
		}
	}
	rtems_interrupt_enable(key);
}
