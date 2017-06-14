/*
 * MIT License
 *
 * Copyright (c) 2017 Max Groening
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#ifndef EMBEDDED_BOOTLOADER_EMBEDDED_BOOTLOADER_H_
#define EMBEDDED_BOOTLOADER_EMBEDDED_BOOTLOADER_H_

#include <stdint.h>
#include "devices/devices.h"

typedef enum {ebh_device_msp430_flash, ebh_device_msp430_fram, ebh_device_msp432} ebh_device;

void ebh_invoke_sequence(void);

void ebh_sync_character(void);

void ebh_delay_between_commands(void);

uint8_t ebh_format_package(uint8_t cmd, uint8_t a_len, uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t  *payload, uint16_t length);

uint8_t ebh_receive_ack();

uint8_t ebh_receive_core_response(uint8_t *payload, uint16_t max_buffer);

uint8_t ebh_rx_data_block(uint32_t addr, uint8_t *data, uint16_t length);
uint8_t ebh_rx_data_block_32(uint32_t addr, uint8_t *data, uint16_t length);

uint8_t ebh_rx_password(uint8_t *data);
uint8_t ebh_rx_password_32(uint8_t *data);

uint8_t ebh_erase_segment(uint32_t addr);
uint8_t ebh_erase_segment_32(uint32_t addr);

/* ebh_unlock_and_lock_info(void) unlocks the write protection of the INFO A segment. For MSP430 with flash memory only. */
uint8_t ebh_unlock_and_lock_info(void);

uint8_t ebh_mass_erase(ebh_device device);

uint8_t ebh_reboot_reset(void);

uint8_t ebh_crc_check(uint32_t addr, uint16_t length, uint16_t *data);
uint8_t ebh_crc_check_32(uint32_t addr, uint16_t length, uint16_t *data);

uint8_t ebh_load_pc(uint32_t addr);
uint8_t ebh_load_pc_32(uint32_t addr);

uint8_t ebh_tx_bsl_version(ebh_device device, uint8_t *data);

uint8_t ebh_factory_reset(uint8_t *data);

uint8_t ebh_change_baud_rate(uint8_t baud_rate);

#endif /* EMBEDDED_BOOTLOADER_EMBEDDED_BOOTLOADER_H_ */
