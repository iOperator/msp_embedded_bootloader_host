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


#include <stdint.h>
#include <stdbool.h>

/*
 * Device depended includes
 */

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"


/*
 * General device initialization and support functions
 */

void ebh_device_init(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);  // Configure to use external 16 MHz clock.
}

void ebh_delay_100_us(void) {
    SysCtlDelay(SysCtlClockGet() / 1000000 * 100 / 3);
}

void ebh_delay_us(uint16_t time) {
    SysCtlDelay(SysCtlClockGet() / 1000000u * time / 3u);
}


/*
 * UART peripheral interface - polling based
 * UART - U1
 * RX - PB0
 * TX - PB1
 */

void ebh_uart_poll_init() {
    // Enable UART1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));

    // Map GPIOs to UART module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

void ebh_uart_poll_configure_9600_baud() {
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_EVEN));
}

void ebh_uart_poll_configure_115200_baud() {
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_EVEN));
}

void ebh_uart_poll_send_char(uint8_t character) {
    UARTCharPut(UART1_BASE, (unsigned char)character);
}

uint8_t ebh_uart_poll_receive_char() {
    return (uint8_t)UARTCharGet(UART1_BASE);
}

uint16_t ebh_uart_poll_receive_char_available() {
    return (uint16_t)UARTCharsAvail(UART1_BASE);
}


/*
 * Reset and Test pin configuration
 * for MSP430 entry sequence
 * RST - PA6
 * TST - PA7
 */

#define EBH_RST_PIN   GPIO_PIN_6
#define EBH_TEST_PIN  GPIO_PIN_7

void ebh_invoke_seqence_pre(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, EBH_RST_PIN | EBH_TEST_PIN);
}

void ebh_invoke_seqence_post(void) {
    // No post-invoke sequence actions needed here
}

void ebh_rst_pin_high(void) {
    GPIOPinWrite(GPIO_PORTA_BASE, EBH_RST_PIN, EBH_RST_PIN);
}

void ebh_rst_pin_low(void) {
    GPIOPinWrite(GPIO_PORTA_BASE, EBH_RST_PIN, 0);
}

void ebh_test_pin_high() {
    GPIOPinWrite(GPIO_PORTA_BASE, EBH_TEST_PIN, EBH_TEST_PIN);
}

void ebh_test_pin_low() {
    GPIOPinWrite(GPIO_PORTA_BASE, EBH_TEST_PIN, 0);
}

/*
 * CRC CCITT algorithm
 */

const uint16_t polynom = 0x1021;
uint16_t crc = 0;

void ebh_crc_init(void) {
    crc = 0xffff;
}

void ebh_crc(uint8_t data) {
    uint_fast8_t i;

    for (i = 0; i < 8; i++) {
        if (((crc & 0x8000) >> 8) ^ (data & 0x80)) {
            crc = (crc << 1) ^ polynom;
        } else {
            crc = (crc << 1);
        }
        data <<= 1;
    }
}

uint16_t ebh_crc_result(void) {
    return crc;
}
