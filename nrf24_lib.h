#pragma once

#include <furi.h>
#include <furi_hal.h>

// Hardware Definitions
#define NRF24_CE_PIN &gpio_ext_pa6
#define NRF24_CS_PIN &gpio_ext_pc3
#define NRF24_HANDLE &furi_hal_spi_bus_handle_external

// Register Definitions
#define NRF24_REG_CONFIG 0x00
#define NRF24_REG_SETUP_AW 0x03
#define NRF24_REG_RF_CH 0x05
#define NRF24_REG_RF_SETUP 0x06
#define NRF24_REG_STATUS 0x07
#define NRF24_REG_RPD 0x09

// Command Definitions
#define NRF24_CMD_READ_REG 0x00
#define NRF24_CMD_WRITE_REG 0x20
#define NRF24_CMD_FLUSH_RX 0xE2
#define NRF24_CMD_W_TX_PAYLOAD 0xA0
#define NRF24_CMD_W_TX_PAYLOAD_NOACK 0xB0
#define MAX_RT (1 << 4)

// Module status structure
typedef struct {
    bool initialized;
    uint8_t last_status;
} NRF24;

/**
 * @brief Initialize the NRF24L01+ module
 * 
 * @param nrf24 Pointer to NRF24 structure
 * @return true if initialization was successful
 */
bool nrf24_init(NRF24* nrf24);

/**
 * @brief Deinitialize the NRF24L01+ module
 * 
 * @param nrf24 Pointer to NRF24 structure
 */
void nrf24_deinit(NRF24* nrf24);

/**
 * @brief Read a register from the NRF24L01+
 * 
 * @param reg Register address
 * @return uint8_t Register value
 */
uint8_t nrf24_read_register(uint8_t reg);

/**
 * @brief Write a value to a register in the NRF24L01+
 * 
 * @param reg Register address
 * @param value Value to write
 */
void nrf24_write_register(uint8_t reg, uint8_t value);

/**
 * @brief Set the NRF24L01+ to idle state (power down)
 */
void nrf24_set_idle(void);

/**
 * @brief Check if the NRF24L01+ is connected and responding
 * 
 * @return true if connected, false otherwise
 */
bool nrf24_check_connection(void);

/**
 * @brief Set the RF channel
 * 
 * @param channel Channel number (0-125)
 */
void nrf24_set_channel(uint8_t channel);

/**
 * @brief Write payload to be transmitted
 * 
 * @param data Pointer to data buffer
 * @param length Length of data (max 32 bytes)
 */
void nrf24_write_payload(uint8_t* data, uint8_t length);

/**
 * @brief Write payload to be transmitted without auto-acknowledgment
 * 
 * @param data Pointer to data buffer
 * @param length Length of data (max 32 bytes)
 */
void nrf24_write_payload_noack(uint8_t* data, uint8_t length);

/**
 * @brief Set the NRF24L01+ to TX mode
 */
void nrf24_set_tx_mode(void);

/**
 * @brief Set the NRF24L01+ to RX mode
 */
void nrf24_set_rx_mode(void);

/**
 * @brief Flush the RX FIFO
 */
void nrf24_flush_rx(void);

/**
 * @brief Set RF power level
 * 
 * @param power Power level (0-3, where 3 is maximum power)
 */
void nrf24_set_power(uint8_t power);

/**
 * @brief Acquire the SPI bus for the NRF24L01+
 */
void nrf24_spi_acquire(void);

/**
 * @brief Release the SPI bus for the NRF24L01+
 */
void nrf24_spi_release(void);