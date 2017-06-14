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


#ifndef EMBEDDED_BOOTLOADER_BOOTLOADER_PROTOCOL_H_
#define EMBEDDED_BOOTLOADER_BOOTLOADER_PROTOCOL_H_

/*
 * BSL commands
 */

#define EBH_CMD_RX_DATA_BLOCK         0x10
#define EBH_CMD_RX_DATA_BLOCK_32      0x20
#define EBH_CMD_RX_DATA_BLOCK_FAST    0x1B
#define EBH_CMD_RX_PASSWORD           0x11
#define EBH_CMD_RX_PASSWORD_32        0x21
#define EBH_CMD_ERASE_SEGMENT         0x12
#define EBH_CMD_ERASE_SEGMENT_32      0x22
#define EBH_CMD_UNLOCK_AND_LOCK_INFO  0x13
#define EBH_CMD_RESERVED              0x14
#define EBH_CMD_MASS_ERASE            0x15
#define EBH_CMD_REBOOT_RESET          0x25
#define EBH_CMD_CRC_CHECK             0x16
#define EBH_CMD_CRC_CHECK_32          0x26
#define EBH_CMD_LOAD_PC               0x17
#define EBH_CMD_LOAD_PC_32            0x27
#define EBH_CMD_TX_DATA_BLOCK         0x18
#define EBH_CMD_TX_DATA_BLOCK_32      0x28
#define EBH_CMD_TX_BSL_VERSION        0x19
#define EBH_CMD_TX_BUFFER_SIZE        0x1A
#define EBH_CMD_FACTORY_RESET         0x30
#define EBH_CMD_CHANGE_BAUD_RATE      0x52

/*
 * BSL core messages
 */

#define EBH_CORE_MSG_DATA     0x3A
#define EBH_CORE_MSG_MESSAGE  0x3B

#define EBH_CORE_MSG_OPERATION_SUCCESSFUL  0x00
#define EBH_CORE_MSG_BSL_LOCKED            0x04
#define EBH_CORE_MSG_BSL_PASSWORD_ERROR    0x05
#define EBH_CORE_MSG_UNKNOWN_COMMAND       0x07

/*
 * UART error messages
 */

#define EBH_UART_ERROR_ACK                         0x00
#define EBH_UART_ERROR_HEADER_INCORRECT            0x51
#define EBH_UART_ERROR_CHECKSUM_INCORRECT          0x52
#define EBH_UART_ERROR_PACKET_SIZE_ZERO            0x53
#define EBH_UART_ERROR_PACKET_SIZE_EXCEEDS_BUFFER  0x54
#define EBH_UART_ERROR_UNKNOWN_ERROR               0x55
#define EBH_UART_ERROR_UNKNOWN_BAUD_RATE           0x56
#define EBH_UART_ERROR_TIME_OUT                    0xEE

/*
 * UART baud rates
 */

#define EBH_UART_BAUD_RATE_9600    0x02
#define EBH_UART_BAUD_RATE_19200   0x03
#define EBH_UART_BAUD_RATE_38400   0x04
#define EBH_UART_BAUD_RATE_56700   0x05
#define EBH_UART_BAUD_RATE_115200  0x06

/*
 * Other
 */

#define EBH_HEADER           0x80  // BSL protocol header byte
#define EBH_MAX_BUFFER_SIZE  262
#define EBH_SYNC_CHARACTER   0xFF  // Sync char used for MSP432 automatic baud rate detection
#define EBH_DELAY_BETWEEN_COMMANDS  1200  // Time between BSL commands in microseconds
#define EBH_ACK_RETRIES      1000  // Number of total retires for ACK
#define EBH_ACK_RETRY_DELAY  10    // Delay in us until checking for ACK again

#endif /* EMBEDDED_BOOTLOADER_BOOTLOADER_PROTOCOL_H_ */
