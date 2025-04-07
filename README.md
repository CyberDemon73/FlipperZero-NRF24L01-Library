# NRF24L01+ Library for Flipper Zero

A lightweight C library for interfacing with NRF24L01+ 2.4GHz RF transceivers on the Flipper Zero platform.

## Overview

This library provides a simple API for communicating with NRF24L01+ modules through the Flipper Zero's external SPI bus. It handles initialization, configuration, and basic transmit/receive functionality.

## Features

- Hardware SPI communication with NRF24L01+ modules
- Easy initialization and configuration
- Support for both standard and no-acknowledgment transmission modes
- Transmit and receive mode switching
- Power level control
- Channel selection

## Hardware Connections

The library is configured for the following pin connections:

- **CE Pin**: External Port Pin PA6
- **CSN Pin**: External Port Pin PC3
- **SPI Bus**: Flipper Zero External SPI Bus

## API Reference

### Initialization and Setup

```c
bool nrf24_init(NRF24* nrf24);
```
Initializes the NRF24L01+ module and verifies communication. Returns `true` if successful.

```c
void nrf24_deinit(NRF24* nrf24);
```
Puts the module in idle mode and releases hardware resources.

```c
bool nrf24_check_connection(void);
```
Verifies that the module is responding correctly. Returns `true` if connected.

### Configuration

```c
void nrf24_set_channel(uint8_t channel);
```
Sets the RF channel (frequency). Valid range: 0-125, corresponding to 2.400-2.525 GHz.

```c
void nrf24_set_power(uint8_t power);
```
Sets the RF output power level:
- 0: -18dBm
- 1: -12dBm
- 2: -6dBm
- 3: 0dBm (maximum)

### Mode Control

```c
void nrf24_set_tx_mode(void);
```
Configures the module for transmission mode.

```c
void nrf24_set_rx_mode(void);
```
Configures the module for reception mode.

```c
void nrf24_set_idle(void);
```
Puts the module in power-down (idle) state to conserve power.

### Data Transmission

```c
void nrf24_write_payload(uint8_t* data, uint8_t length);
```
Sends a data payload with auto-acknowledgment (if enabled). Maximum length: 32 bytes.

```c
void nrf24_write_payload_noack(uint8_t* data, uint8_t length);
```
Sends a data payload without waiting for acknowledgment. Maximum length: 32 bytes.

### Low-Level Functions

```c
uint8_t nrf24_read_register(uint8_t reg);
void nrf24_write_register(uint8_t reg, uint8_t value);
```
Read or write to the module's registers directly.

```c
void nrf24_flush_rx(void);
```
Flushes the receive FIFO buffer.

```c
void nrf24_spi_acquire(void);
void nrf24_spi_release(void);
```
Acquire or release the SPI bus for transaction sequences.

## Example Usage

```c
// Initialize module
NRF24 nrf24;
if (!nrf24_init(&nrf24)) {
    // Handle initialization error
    return;
}

// Configure module
nrf24_spi_acquire();
nrf24_set_channel(76);  // Set to channel 76 (2.476 GHz)
nrf24_set_power(3);     // Set to maximum power
nrf24_set_tx_mode();    // Set to transmit mode
nrf24_spi_release();

// Send data
uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
nrf24_spi_acquire();
nrf24_write_payload(data, 4);
nrf24_spi_release();

// Clean up when done
nrf24_deinit(&nrf24);
```

## Implementation Notes

- The library assumes the NRF24L01+ is connected to the Flipper Zero's external SPI bus.
- SPI bus must be acquired before performing operations and released afterward.
- The module's connection is verified by checking if the SETUP_AW register returns the expected value (0x03).
- Transmission is triggered by pulsing the CE pin high for 15μs after loading the payload.

## Dependencies

- Flipper Zero Firmware
- `furi.h` and `furi_hal.h` libraries

## Safety and Educational Disclaimer

**IMPORTANT:** This library is provided for educational and experimental purposes only. Users should be aware of and comply with all local laws and regulations regarding radio frequency transmissions. Improper use of RF transmitters may interfere with other electronic devices and can be illegal in many jurisdictions.

- This library allows control of radio frequency transmissions which may be regulated in your area
- The user is solely responsible for ensuring compliance with local regulations
- The authors do not accept responsibility for any misuse or regulatory violations
- Always operate within legal frequency bands and power limits for your location
- Be respectful of others using the radio spectrum

## License

This library is released under the MIT License, making it free to use, modify, and distribute:

```
MIT License

Copyright (c) 2025 [Mohamed Hisham/CyberDemon73]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
