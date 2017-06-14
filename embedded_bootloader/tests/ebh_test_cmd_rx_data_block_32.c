/*
 * MIT License
 *
 * Copyright (c) 2017 Max Groening
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


#include <stdint.h>
#include <stdbool.h>

#include "embedded_bootloader/embedded_bootloader.h"
#include "embedded_bootloader/bootloader_protocol.h"
#include "embedded_bootloader/tests/test_support.h"


#define TEST_MSP432  // Only applicable to MSP432

uint16_t test_pass = 0;
uint16_t test_fail = 0;
uint16_t test_total = 0;
uint8_t status = 0;

void main(void) {

    ebh_device_init();

#ifdef TEST_MSP430
    ebh_invoke_sequence();
#endif

    ebh_uart_poll_init();
    ebh_uart_poll_configure_9600_baud();

#ifdef TEST_MSP432
    ebh_sync_character();
#endif

    ebh_change_baud_rate(0x06);
    ebh_delay_between_commands();

    ebh_uart_poll_configure_115200_baud();

    /* Locked BSL shall respond with BSL LOCKED  */
    status = ebh_rx_data_block_32(0x20001080, payload0, sizeof(payload0));
    if(status != EBH_CORE_MSG_BSL_LOCKED) {
        test_fail++;
    } else {
        test_pass++;
    }
    test_total++;
    ebh_delay_between_commands();

    /* Unlock BSL */
#ifdef TEST_MSP430
    status = ebh_rx_password(password_empty_msp430);
#endif
#ifdef TEST_MSP432
    status = ebh_rx_password_32(password_empty_msp432);
#endif
    if(status != EBH_CORE_MSG_OPERATION_SUCCESSFUL) {
        test_fail++;
    } else {
        test_pass++;
    }
    test_total++;
    ebh_delay_between_commands();

    /* Transfer 1 byte */
    status = ebh_rx_data_block_32(0x20001080, payload0, sizeof(payload0));
    if(status != EBH_CORE_MSG_OPERATION_SUCCESSFUL) {
        test_fail++;
    } else {
        test_pass++;
    }
    test_total++;
    ebh_delay_between_commands();

    /* Transfer 16 bytes */
    status = ebh_rx_data_block_32(0x20001080, payload1, sizeof(payload1));
    if(status != EBH_CORE_MSG_OPERATION_SUCCESSFUL) {
        test_fail++;
    } else {
        test_pass++;
    }
    test_total++;
    ebh_delay_between_commands();

    /* Transfer 256 bytes */
    status = ebh_rx_data_block_32(0x20001080, payload2, sizeof(payload2));
    if(status != EBH_CORE_MSG_OPERATION_SUCCESSFUL) {
        test_fail++;
    } else {
        test_pass++;
    }
    test_total++;
    ebh_delay_between_commands();

    /* Transfer 513 bytes */
    status = ebh_rx_data_block_32(0x20001080, payload3, sizeof(payload3));
    if(status != EBH_CORE_MSG_OPERATION_SUCCESSFUL) {
        test_fail++;
    } else {
        test_pass++;
    }
    test_total++;
    ebh_delay_between_commands();

    while(1);
}
