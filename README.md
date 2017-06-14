# MSP Embedded Bootloader Host

This project aims to provide an embedded host for the Texas Instruments MSP430 and MSP432 Bootloader (BSL). You can run this software for example on a microcontroller and program (i.e. initial programming or updating) a connected MSP430 or MSP432 via the TI bootloader.

## Features

  * written with portability and hardware abstraction in mind
  * supports TI BSL protocoll for MSP432, MSP430 5xx/6xx Flash and FRAM series
  * supports UART peripheral interface so far (I<sup>2</sup>C and SPI are t.b.d.)
  * comes with an example running on TI TM4C123 LaunchPad (other devices are t.b.d.)
  * written in C language

**Supported BSL commands**  
The commands with a checkmark are currently supported.

  - [x] RX_DATA_BLOCK
  - [x] RX_DATA_BLOCK_32
  - [ ] EBH_CMD_RX_DATA_BLOCK_FAST
  - [x] EBH_CMD_RX_PASSWORD
  - [x] EBH_CMD_RX_PASSWORD_32
  - [x] EBH_CMD_ERASE_SEGMENT
  - [x] EBH_CMD_ERASE_SEGMENT_32
  - [x] EBH_CMD_UNLOCK_AND_LOCK_INFO
  - [x] EBH_CMD_MASS_ERASE
  - [x] EBH_CMD_REBOOT_RESET
  - [x] EBH_CMD_CRC_CHECK
  - [x] EBH_CMD_CRC_CHECK_32
  - [x] EBH_CMD_LOAD_PC
  - [x] EBH_CMD_LOAD_PC_32
  - [ ] EBH_CMD_TX_DATA_BLOCK
  - [ ] EBH_CMD_TX_DATA_BLOCK_32
  - [x] EBH_CMD_TX_BSL_VERSION
  - [ ] EBH_CMD_TX_BUFFER_SIZE
  - [x] EBH_CMD_FACTORY_RESET
  - [x] EBH_CMD_CHANGE_BAUD_RATE (currently only 9600 and 115200)

## Code Example

```C
#include "embedded_bootloader/embedded_bootloader.h"

void main(void) {
  ebh_device_init();  // Initialize the host device
  ebh_uart_poll_init();  // Initialize the communication peripheral on the host device
  ebh_uart_poll_configure_9600_baud();  // Set UART to 9600 baud

  ebh_invoke_sequence();  // Invoke sequence for MSP430 devices
  if(ebh_rx_password(password_empty_msp430)) {/*error handling*/}  // Send password
  ebh_delay_between_commands();  // Wait for recommended time between commands
  if(ebh_rx_data_block(0x8000, payload1, sizeof(payload1))) {/*error handling*/}  // Send firmware
  // ...
}
```

## Installation

  * Copy the `embedded_bootloader` folder into your project folder.
  * Include the MSP Embedded Bootloader Host header (`#include "embedded_bootloader/embedded_bootloader.h"`)
  * Implement the functions referenced in the board support package header `embedded_bootloader/devices/devices.h` for your host device. (You can refer to `embedded_bootloader/devices/bsp_tm4c123gh6pm.c`)
  * (Exclude the tests (in `embedded_bootloader/tests`) from your project)


## API Reference

| Function | Desciption |
| --- | --- |
| `void ebh_invoke_sequence(void)` | Generates MSP430 invoke sequence. |
| `void ebh_sync_character(void)` | Send the UART sync character for MSP432. |
| `void ebh_delay_between_commands(void)` | Waits for the recommended amount of time between two BSL commands. |
| `uint8_t ebh_rx_data_block(uint32_t addr, uint8_t *data, uint16_t length)` | Programs `data` of given `length` at address `addr`. |
| `uint8_t ebh_rx_data_block_32(uint32_t addr, uint8_t *data, uint16_t length)` | Programs `data` of given `length` at address `addr`. Supports 32-bit addresses for MSP432. |
| `uint8_t ebh_rx_password(uint8_t *data)` | Sends the given 16 bytes password. (MSP430) |
| `uint8_t ebh_rx_password_32(uint8_t *data)` | Sends the given 256 bytes password. (MSP432) |
| `uint8_t ebh_erase_segment(uint32_t addr)` | Erases the flash segment at address `addr`. |
| `uint8_t ebh_erase_segment_32(uint32_t addr)` | Erases the flash segment at address `addr`. Supports 32-bit addresses for MSP432. |
| `uint8_t ebh_unlock_and_lock_info(void)` | Unlocks the write protection of the INFO A segment. For MSP430 with flash memory only. |
| `uint8_t ebh_mass_erase(ebh_device device)` | Mass erases the device.  |
| `uint8_t ebh_reboot_reset(void)` | Triggers a reboot reset. (MSP432) |
| `uint8_t ebh_crc_check(uint32_t addr, uint16_t length, uint16_t *data)` | Receives the CRC16 checksum of the data at address `addr` with length `length` from the BSL target and stores it at `data`. |
| `uint8_t ebh_crc_check_32(uint32_t addr, uint16_t length, uint16_t *data)` | Receives the CRC16 checksum of the data at address `addr` with length `length` from the BSL target and stores it at `data`. Supports 32-bit addresses for MSP432. |
| `uint8_t ebh_load_pc(uint32_t addr)` | Sets the Program Counter on the BSL target. |
| `uint8_t ebh_load_pc_32(uint32_t addr)` | Sets the Program Counter on the BSL target. Supports 32-bit addresses for MSP432. |
| `uint8_t ebh_tx_bsl_version(ebh_device device, uint8_t *data)` | Receives the BSL version from the target and stores it at `data`. |
| `uint8_t ebh_factory_reset(uint8_t *data)` | Triggers a factory reset of the MSP432 target using the password at `data`. |
| `uint8_t ebh_change_baud_rate(uint8_t baud_rate)` | Changes the UART baud rate of the BSL target. |

## Tests

Currently tested with a MSP432 device. Some tests files are located in `embedded_bootloader/tests`.

## Licence

This project is released under the MIT license:


MIT License

Copyright (c) 2017 Max Groening

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
