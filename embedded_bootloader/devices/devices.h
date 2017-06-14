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


#ifndef EMBEDDED_BOOTLOADER_DEVICES_DEVICES_H_
#define EMBEDDED_BOOTLOADER_DEVICES_DEVICES_H_


/*
 * General device initialization and support function
 */

void ebh_device_init(void);
void ebh_delay_100_us(void);
void ebh_delay_us(uint16_t time);

/*
 * UART (polling) interface
 */

void ebh_uart_poll_init();
void ebh_uart_poll_configure_9600_baud();
void ebh_uart_poll_configure_115200_baud();
void ebh_uart_poll_send_char(uint8_t character);
uint8_t ebh_uart_poll_receive_char();
uint16_t ebh_uart_poll_receive_char_available();

/*
 * Invoke sequence, RST and TST pin
 */

void ebh_invoke_seqence_pre(void);
void ebh_invoke_seqence_post(void);
void ebh_rst_pin_high(void);
void ebh_rst_pin_low(void);
void ebh_test_pin_high(void);
void ebh_test_pin_low(void);

/*
 * CRC CCITT algorithm
 */

void ebh_crc_init(void);
void ebh_crc(uint8_t data);
uint16_t ebh_crc_result(void);

void ebh_send_char(uint8_t character);
uint8_t ebh_receive_char();
uint16_t ebh_receive_char_available();

#endif /* EMBEDDED_BOOTLOADER_DEVICES_DEVICES_H_ */
