/*******************************************************************************

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

*******************************************************************************/
/*$FreeBSD: src/sys/dev/em/e1000_api.h,v 1.3 2007/05/16 00:14:23 jfv Exp $*/


#ifndef _E1000_API_H_
#define _E1000_API_H_

#include "e1000_hw.h"

s32       e1000_set_mac_type(struct e1000_hw *hw);
s32       e1000_setup_init_funcs(struct e1000_hw *hw, boolean_t init_device);
s32       e1000_init_mac_params(struct e1000_hw *hw);
s32       e1000_init_nvm_params(struct e1000_hw *hw);
s32       e1000_init_phy_params(struct e1000_hw *hw);
void      e1000_remove_device(struct e1000_hw *hw);
s32       e1000_get_bus_info(struct e1000_hw *hw);
void      e1000_clear_vfta(struct e1000_hw *hw);
void      e1000_write_vfta(struct e1000_hw *hw, u32 offset, u32 value);
s32       e1000_force_mac_fc(struct e1000_hw *hw);
s32       e1000_check_for_link(struct e1000_hw *hw);
s32       e1000_reset_hw(struct e1000_hw *hw);
s32       e1000_init_hw(struct e1000_hw *hw);
s32       e1000_setup_link(struct e1000_hw *hw);
s32       e1000_get_speed_and_duplex(struct e1000_hw *hw, u16 *speed,
                                     u16 *duplex);
s32       e1000_disable_pcie_master(struct e1000_hw *hw);
void      e1000_config_collision_dist(struct e1000_hw *hw);
void      e1000_rar_set(struct e1000_hw *hw, u8 *addr, u32 index);
void      e1000_mta_set(struct e1000_hw *hw, u32 hash_value);
u32       e1000_hash_mc_addr(struct e1000_hw *hw, u8 *mc_addr);
void      e1000_mc_addr_list_update(struct e1000_hw *hw,
                                    u8 *mc_addr_list, u32 mc_addr_count,
                                    u32 rar_used_count, u32 rar_count);
s32       e1000_setup_led(struct e1000_hw *hw);
s32       e1000_cleanup_led(struct e1000_hw *hw);
s32       e1000_check_reset_block(struct e1000_hw *hw);
s32       e1000_blink_led(struct e1000_hw *hw);
s32       e1000_led_on(struct e1000_hw *hw);
s32       e1000_led_off(struct e1000_hw *hw);
void      e1000_reset_adaptive(struct e1000_hw *hw);
void      e1000_update_adaptive(struct e1000_hw *hw);
s32       e1000_get_cable_length(struct e1000_hw *hw);
s32       e1000_validate_mdi_setting(struct e1000_hw *hw);
s32       e1000_read_phy_reg(struct e1000_hw *hw, u32 offset, u16 *data);
s32       e1000_write_phy_reg(struct e1000_hw *hw, u32 offset, u16 data);
s32       e1000_write_8bit_ctrl_reg(struct e1000_hw *hw, u32 reg,
                                 u32 offset, u8 data);
s32       e1000_get_phy_info(struct e1000_hw *hw);
s32       e1000_phy_hw_reset(struct e1000_hw *hw);
s32       e1000_phy_commit(struct e1000_hw *hw);
s32       e1000_read_mac_addr(struct e1000_hw *hw);
s32       e1000_read_part_num(struct e1000_hw *hw, u32 *part_num);
void      e1000_reload_nvm(struct e1000_hw *hw);
s32       e1000_update_nvm_checksum(struct e1000_hw *hw);
s32       e1000_validate_nvm_checksum(struct e1000_hw *hw);
s32       e1000_read_nvm(struct e1000_hw *hw, u16 offset, u16 words, u16 *data);
s32       e1000_read_kmrn_reg(struct e1000_hw *hw, u32 offset, u16 *data);
s32       e1000_write_kmrn_reg(struct e1000_hw *hw, u32 offset, u16 data);
s32       e1000_write_nvm(struct e1000_hw *hw, u16 offset, u16 words,
                          u16 *data);
s32       e1000_wait_autoneg(struct e1000_hw *hw);
s32       e1000_set_d3_lplu_state(struct e1000_hw *hw, boolean_t active);
s32       e1000_set_d0_lplu_state(struct e1000_hw *hw, boolean_t active);
boolean_t e1000_check_mng_mode(struct e1000_hw *hw);
boolean_t e1000_enable_mng_pass_thru(struct e1000_hw *hw);
boolean_t e1000_enable_tx_pkt_filtering(struct e1000_hw *hw);
s32       e1000_mng_enable_host_if(struct e1000_hw *hw);
s32       e1000_mng_host_if_write(struct e1000_hw *hw,
                                  u8 *buffer, u16 length, u16 offset, u8 *sum);
s32       e1000_mng_write_cmd_header(struct e1000_hw *hw,
                                     struct e1000_host_mng_command_header *hdr);
s32       e1000_mng_write_dhcp_info(struct e1000_hw * hw,
                                    u8 *buffer, u16 length);
void      e1000_tbi_adjust_stats_82543(struct e1000_hw *hw,
                                       struct e1000_hw_stats *stats,
                                       u32 frame_len, u8 *mac_addr);
void      e1000_set_tbi_compatibility_82543(struct e1000_hw *hw,
                                            boolean_t state);
boolean_t e1000_tbi_sbp_enabled_82543(struct e1000_hw *hw);
#ifndef NO_82542_SUPPORT
u32       e1000_translate_register_82542(u32 reg);
#endif
void      e1000_init_script_state_82541(struct e1000_hw *hw, boolean_t state);
boolean_t e1000_get_laa_state_82571(struct e1000_hw *hw);
void      e1000_set_laa_state_82571(struct e1000_hw *hw, boolean_t state);
#ifndef NO_ICH8LAN_SUPPORT
void      e1000_set_kmrn_lock_loss_workaround_ich8lan(struct e1000_hw *hw,
                                                      boolean_t state);
void      e1000_igp3_phy_powerdown_workaround_ich8lan(struct e1000_hw *hw);
void      e1000_gig_downshift_workaround_ich8lan(struct e1000_hw *hw);
#endif


/* TBI_ACCEPT macro definition:
 *
 * This macro requires:
 *      adapter = a pointer to struct e1000_hw
 *      status = the 8 bit status field of the RX descriptor with EOP set
 *      error = the 8 bit error field of the RX descriptor with EOP set
 *      length = the sum of all the length fields of the RX descriptors that
 *               make up the current frame
 *      last_byte = the last byte of the frame DMAed by the hardware
 *      max_frame_length = the maximum frame length we want to accept.
 *      min_frame_length = the minimum frame length we want to accept.
 *
 * This macro is a conditional that should be used in the interrupt
 * handler's Rx processing routine when RxErrors have been detected.
 *
 * Typical use:
 *  ...
 *  if (TBI_ACCEPT) {
 *      accept_frame = TRUE;
 *      e1000_tbi_adjust_stats(adapter, MacAddress);
 *      frame_length--;
 *  } else {
 *      accept_frame = FALSE;
 *  }
 *  ...
 */

/* The carrier extension symbol, as received by the NIC. */
#define CARRIER_EXTENSION   0x0F

#define TBI_ACCEPT(a, status, errors, length, last_byte) \
    (e1000_tbi_sbp_enabled_82543(a) && \
     (((errors) & E1000_RXD_ERR_FRAME_ERR_MASK) == E1000_RXD_ERR_CE) && \
     ((last_byte) == CARRIER_EXTENSION) && \
     (((status) & E1000_RXD_STAT_VP) ? \
          (((length) > ((a)->mac.min_frame_size - VLAN_TAG_SIZE)) && \
           ((length) <= ((a)->mac.max_frame_size + 1))) : \
          (((length) > (a)->mac.min_frame_size) && \
           ((length) <= ((a)->mac.max_frame_size + VLAN_TAG_SIZE + 1)))))

#endif
