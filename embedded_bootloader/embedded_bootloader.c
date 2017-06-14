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
#include "embedded_bootloader.h"
#include "devices/devices.h"
#include "embedded_bootloader/bootloader_protocol.h"


void ebh_invoke_sequence(void) {

    /*
     *      H -------+           +--------
     * RST  L        +-----------+
     *
     *      H ----+     +--+  +-----+
     * TEST L     +-----+  +--+     +-----
     *
     */

    // Setup the GPIOs
    ebh_invoke_seqence_pre();

    // Initially RST and TEST are high
    ebh_rst_pin_high();
    ebh_test_pin_high();
    ebh_delay_us(200);

    // TEST goes low (> 100 us)
    ebh_test_pin_low();
    ebh_delay_us(110);

    // RST goes low (> 2 us)
    ebh_rst_pin_low();
    ebh_delay_us(5);

    // TEST goes high (> 110 us)
    ebh_test_pin_high();
    ebh_delay_us(120);

    // TEST goes low (< 15 us)
    ebh_test_pin_low();
    ebh_delay_us(10);

    // TEST goes high
    ebh_test_pin_high();
    ebh_delay_us(100);

    // RST goes high
    ebh_rst_pin_high();
    ebh_delay_us(100);

    // TST goes low
    ebh_test_pin_low();
    ebh_delay_us(200);

    ebh_invoke_seqence_post();
}

void ebh_sync_character(void) {
    ebh_send_char(EBH_SYNC_CHARACTER);
    ebh_receive_char();
}

void ebh_delay_between_commands(void) {
    // A 1.2 ms delay is recommended between the BSL commands.
    ebh_delay_us(EBH_DELAY_BETWEEN_COMMANDS);
}

uint8_t ebh_format_package(uint8_t cmd, uint8_t a_len, uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t *payload, uint16_t length) {

    /*
     * HDR   Header (0x80)
     * NL    Length of BSL core data packet (low byte)
     * NH    ... (high byte)
     * [..]  BSL core command
     * CKL   CRC checksum on BSL core data packet (low byte)
     * CKH   ... (high byte)
     */

    uint8_t nl = (length + 1 + a_len) & 0xFF;
    uint8_t nh = ((length + 1 + a_len) >> 8) & 0xFF;
    uint16_t crc = 0;

    ebh_send_char(EBH_HEADER);
    ebh_send_char(nl);
    ebh_send_char(nh);
    ebh_send_char(cmd);
    ebh_crc_init();
    ebh_crc(cmd);

    if(a_len > 0) {
        ebh_send_char(a0);
        ebh_crc(a0);
    }
    if(a_len > 1) {
        ebh_send_char(a1);
        ebh_crc(a1);
    }
    if(a_len > 2) {
        ebh_send_char(a2);
        ebh_crc(a2);
    }
    if(a_len > 3) {
        ebh_send_char(a3);
        ebh_crc(a3);
    }

    if(length > 0) {
        uint16_t i = 0;
        for(i = 0; i < length; i++) {
            ebh_send_char(payload[i]);
            ebh_crc(payload[i]);
        }
    }

    crc = ebh_crc_result();
    ebh_send_char(crc & 0xFF);
    ebh_send_char((crc >> 8) & 0xFF);

    return 0;
}

uint8_t ebh_receive_ack() {
    uint_fast16_t i = 0;
    for (i = 0; i < EBH_ACK_RETRIES; i++) {
        ebh_delay_us(EBH_ACK_RETRY_DELAY);
        if(ebh_receive_char_available()) {
            return ebh_receive_char();
        }
    }
    return EBH_UART_ERROR_TIME_OUT;
}

uint8_t ebh_receive_core_response(uint8_t *payload, uint16_t max_buffer) {
    // Header
    uint8_t header = ebh_receive_char();
    if(header != EBH_HEADER) {
        return EBH_UART_ERROR_HEADER_INCORRECT;
    }
    // Length
    uint16_t length = ebh_receive_char();
    length += ebh_receive_char() << 8;
    // Usually EBH_MAX_BUFFER_SIZE would be allowed here,
    // but we can prevent buffer overrun if smaller rx buffer is used.
    if(length > max_buffer) {
        return EBH_UART_ERROR_PACKET_SIZE_EXCEEDS_BUFFER;
    }

    // Core response
    uint_fast16_t i = 0;
    uint_fast8_t character = 0;
    ebh_crc_init();
    for(i = 0; i < length; i++) {
        character = ebh_receive_char();
        payload[i] = character;
        ebh_crc(character);
    }

    // CRC
    uint16_t crc = ebh_receive_char();
    crc += ebh_receive_char() << 8;

    if(crc != ebh_crc_result()) {
        return EBH_UART_ERROR_CHECKSUM_INCORRECT;
    }
    return EBH_UART_ERROR_ACK;
}



uint8_t ebh_rx_data_block(uint32_t addr, uint8_t *data, uint16_t length) {
    uint8_t ack = 0;
    uint8_t rx_buf[2];  // This command expects no core response message bigger than 2.

    uint16_t offset = 0;
    uint8_t a0 = addr & 0xFF;
    uint8_t a1 = (addr >> 8) & 0xFF;
    uint8_t a2 = (addr >> 16) & 0xFF;

    while(length > 256) {
        ebh_format_package(EBH_CMD_RX_DATA_BLOCK, 3, a0, a1, a2, 0, &data[offset], 256u);
        ack = ebh_receive_ack();
        if(ack != EBH_UART_ERROR_ACK) {
            return ack;
        }
        ebh_receive_core_response(rx_buf, 2);
        if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
            return rx_buf[1];
        }
        offset += 256;
        length -= 256;
        a1 = ((addr + offset) >> 8) & 0xFF;
        a2 = ((addr + offset) >> 16) & 0xFF;
    }
    ebh_format_package(EBH_CMD_RX_DATA_BLOCK, 3, a0, a1, a2, 0, &data[offset], length);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 2);
    if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
        return rx_buf[1];
    }
    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_rx_data_block_32(uint32_t addr, uint8_t *data, uint16_t length) {
    uint8_t ack = 0;
    uint8_t rx_buf[2];  // This command expects no core response message bigger than 2.

    uint16_t offset = 0;
    uint8_t a0 = addr & 0xFF;
    uint8_t a1 = (addr >> 8) & 0xFF;
    uint8_t a2 = (addr >> 16) & 0xFF;
    uint8_t a3 = (addr >> 24) & 0xFF;

    while(length > 256) {
        ebh_format_package(EBH_CMD_RX_DATA_BLOCK_32, 4, a0, a1, a2, a3, &data[offset], 256u);
        ack = ebh_receive_ack();
        if(ack != EBH_UART_ERROR_ACK) {
            return ack;
        }
        ebh_receive_core_response(rx_buf, 2);
        if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
            return rx_buf[1];
        }
        offset += 256;
        length -= 256;
        a1 = ((addr + offset) >> 8) & 0xFF;
        a2 = ((addr + offset) >> 16) & 0xFF;
        a3 = ((addr + offset) >> 24) & 0xFF;
    }
    ebh_format_package(EBH_CMD_RX_DATA_BLOCK_32, 4, a0, a1, a2, a3, &data[offset], length);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 2);
    if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
        return rx_buf[1];
    }
    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_rx_password(uint8_t *data) {
    uint8_t ack = 0;
    uint8_t rx_buf[2];  // This command expects no core response message bigger than 2.

    ebh_format_package(EBH_CMD_RX_PASSWORD, 0, 0, 0, 0, 0, data, 32u);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 2);
    if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
        return rx_buf[1];
    }
    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_rx_password_32(uint8_t *data) {
    uint8_t ack = 0;
    uint8_t rx_buf[2];  // This command expects no core response message bigger than 2.

    ebh_format_package(EBH_CMD_RX_PASSWORD_32, 0, 0, 0, 0, 0, data, 256u);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 2);
    if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
        return rx_buf[1];
    }
    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_erase_segment(uint32_t addr) {
    uint8_t ack = 0;
    uint8_t rx_buf[2];  // This command expects no core response message bigger than 2.

    uint8_t a0 = addr & 0xFF;
    uint8_t a1 = (addr >> 8) & 0xFF;
    uint8_t a2 = (addr >> 16) & 0xFF;

    ebh_format_package(EBH_CMD_ERASE_SEGMENT, 3, a0, a1, a2, 0, 0, 0);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 2);
    if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
        return rx_buf[1];
    }
    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_erase_segment_32(uint32_t addr) {
    uint8_t ack = 0;
    uint8_t rx_buf[2];  // This command expects no core response message bigger than 2.

    uint8_t a0 = addr & 0xFF;
    uint8_t a1 = (addr >> 8) & 0xFF;
    uint8_t a2 = (addr >> 16) & 0xFF;
    uint8_t a3 = (addr >> 24) & 0xFF;

    ebh_format_package(EBH_CMD_ERASE_SEGMENT_32, 4, a0, a1, a2, a3, 0, 0);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 2);
    if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
        return rx_buf[1];
    }
    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_unlock_and_lock_info(void) {
    uint8_t ack = 0;
    uint8_t rx_buf[2];  // This command expects no core response message bigger than 2.

    ebh_format_package(EBH_CMD_UNLOCK_AND_LOCK_INFO, 0, 0, 0, 0, 0, 0, 0);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 2);
    if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
        return rx_buf[1];
    }
    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_mass_erase(ebh_device device) {
    uint8_t ack = 0;
    uint8_t rx_buf[2];  // This command expects no core response message bigger than 2.

    ebh_format_package(EBH_CMD_MASS_ERASE, 0, 0, 0, 0, 0, 0, 0);

    // MSP430 FRAM devices do not return a ACK or core message as they reboot on mass erase
    if(device != ebh_device_msp430_fram) {
        ack = ebh_receive_ack();
        if(ack != EBH_UART_ERROR_ACK) {
            return ack;
        }

        ebh_receive_core_response(rx_buf, 2);
        if((rx_buf[0] == EBH_CORE_MSG_MESSAGE) && (rx_buf[1] != EBH_CORE_MSG_OPERATION_SUCCESSFUL)) {
            return rx_buf[1];
        }
    }

    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_reboot_reset(void) {
    ebh_format_package(EBH_CMD_REBOOT_RESET, 0, 0, 0, 0, 0, 0, 0);
    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_crc_check(uint32_t addr, uint16_t length, uint16_t *data) {
    uint8_t ack = 0;
    uint8_t rx_buf[3];  // This command expects no core response message bigger than 3.

    uint8_t a0 = addr & 0xFF;
    uint8_t a1 = (addr >> 8) & 0xFF;
    uint8_t a2 = (addr >> 16) & 0xFF;

    uint8_t len[2];
    len[0] = length & 0xFF;
    len[1] = (length >> 8) & 0xFF;

    ebh_format_package(EBH_CMD_CRC_CHECK, 3, a0, a1, a2, 0, len, 2u);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 3);
    if((rx_buf[0] == EBH_CORE_MSG_DATA)) {
        *data = rx_buf[1] + (rx_buf[2] << 8);
    } else {  // Error case
        return rx_buf[1];
    }

    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_crc_check_32(uint32_t addr, uint16_t length, uint16_t *data) {
    uint8_t ack = 0;
    uint8_t rx_buf[3];  // This command expects no core response message bigger than 3.

    uint8_t a0 = addr & 0xFF;
    uint8_t a1 = (addr >> 8) & 0xFF;
    uint8_t a2 = (addr >> 16) & 0xFF;
    uint8_t a3 = (addr >> 24) & 0xFF;

    uint8_t len[2];
    len[0] = length & 0xFF;
    len[1] = (length >> 8) & 0xFF;

    ebh_format_package(EBH_CMD_CRC_CHECK_32, 4, a0, a1, a2, a3, len, 2u);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 3);
    if((rx_buf[0] == EBH_CORE_MSG_DATA)) {
        *data = rx_buf[1] + (rx_buf[2] << 8);
    } else {  // Error case
        return rx_buf[1];
    }

    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_load_pc(uint32_t addr) {
    uint8_t ack = 0;

    uint8_t a0 = addr & 0xFF;
    uint8_t a1 = (addr >> 8) & 0xFF;
    uint8_t a2 = (addr >> 16) & 0xFF;

    ebh_format_package(EBH_CMD_LOAD_PC, 3, a0, a1, a2, 0, 0, 0);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_load_pc_32(uint32_t addr) {
    uint8_t ack = 0;

    uint8_t a0 = addr & 0xFF;
    uint8_t a1 = (addr >> 8) & 0xFF;
    uint8_t a2 = (addr >> 16) & 0xFF;
    uint8_t a3 = (addr >> 24) & 0xFF;

    ebh_format_package(EBH_CMD_LOAD_PC_32, 4, a0, a1, a2, a3, 0, 0);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_tx_bsl_version(ebh_device device, uint8_t *data) {
    uint8_t ack = 0;
    uint8_t rx_buf[11];  // This command expects no core response message bigger than 11 for MSP432, 5 for MSP430.
                         // Given that at least two conditions more would be required to lower the buffer size for MSP430 only
                         // 'spending' the additional 6 byte seems acceptable.

    ebh_format_package(EBH_CMD_TX_BSL_VERSION, 0, 0, 0, 0, 0, 0, 0);

    ack = ebh_receive_ack();
    if(ack != EBH_UART_ERROR_ACK) {
        return ack;
    }

    ebh_receive_core_response(rx_buf, 11);

    if((rx_buf[0] != EBH_CORE_MSG_DATA)) {  // Error case
        return rx_buf[1];
    } else {
        uint_fast8_t i = 0;
        if(device == ebh_device_msp432) {
            for(i = 0; i < 10; i++) {
                data[i] = rx_buf[i + 1];
            }
        } else {
            for(i = 0; i < 4; i++) {
                data[i] = rx_buf[i + 1];
            }
        }
    }

    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_factory_reset(uint8_t *data) {

    ebh_format_package(EBH_CMD_FACTORY_RESET, 0, 0, 0, 0, 0, data, 16u);

    return EBH_UART_ERROR_ACK;
}

uint8_t ebh_change_baud_rate(uint8_t baud_rate) {
    ebh_format_package(EBH_CMD_CHANGE_BAUD_RATE, 1, baud_rate, 0, 0, 0, 0, 0);
    return ebh_receive_ack();
}
