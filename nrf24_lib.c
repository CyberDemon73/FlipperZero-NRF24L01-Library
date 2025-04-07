#include "nrf24_lib.h"

bool nrf24_init(NRF24* nrf24) {
    if(nrf24 == NULL) return false;

    // Initialize GPIO pins
    furi_hal_gpio_init_simple(NRF24_CS_PIN, GpioModeOutputPushPull);
    furi_hal_gpio_write(NRF24_CS_PIN, true);
    furi_hal_gpio_init(NRF24_CE_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(NRF24_CE_PIN, false);
    
    // Initialize SPI bus
    furi_hal_spi_bus_handle_init(NRF24_HANDLE);
    
    // Check if module is connected
    nrf24_spi_acquire();
    bool connected = nrf24_check_connection();
    nrf24_spi_release();
    
    if(connected) {
        nrf24->initialized = true;
        nrf24->last_status = 0;
    } else {
        nrf24->initialized = false;
    }
    
    return connected;
}

void nrf24_deinit(NRF24* nrf24) {
    if(nrf24 == NULL) return;
    
    // Set to idle state
    nrf24_spi_acquire();
    nrf24_set_idle();
    nrf24_spi_release();
    
    // Deinitialize SPI bus
    furi_hal_spi_bus_handle_deinit(NRF24_HANDLE);
    
    // Reset state
    nrf24->initialized = false;
}

void nrf24_spi_acquire(void) {
    furi_hal_spi_acquire(NRF24_HANDLE);
}

void nrf24_spi_release(void) {
    furi_hal_spi_release(NRF24_HANDLE);
}

uint8_t nrf24_read_register(uint8_t reg) {
    uint8_t tx_data[2] = {NRF24_CMD_READ_REG | reg, 0};
    uint8_t rx_data[2] = {0};
    
    furi_hal_gpio_write(NRF24_CS_PIN, false);
    furi_hal_spi_bus_trx(NRF24_HANDLE, tx_data, rx_data, 2, 100);
    furi_hal_gpio_write(NRF24_CS_PIN, true);
    
    return rx_data[1];
}

void nrf24_write_register(uint8_t reg, uint8_t value) {
    uint8_t tx_data[2] = {NRF24_CMD_WRITE_REG | reg, value};
    
    furi_hal_gpio_write(NRF24_CS_PIN, false);
    furi_hal_spi_bus_tx(NRF24_HANDLE, tx_data, 2, 100);
    furi_hal_gpio_write(NRF24_CS_PIN, true);
}

void nrf24_set_idle(void) {
    // Read current config
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    
    // Clear PWR_UP bit to enter power down mode
    config &= ~(1 << 1);
    nrf24_write_register(NRF24_REG_CONFIG, config);
    
    // Disable CE
    furi_hal_gpio_write(NRF24_CE_PIN, false);
}

bool nrf24_check_connection(void) {
    return (nrf24_read_register(NRF24_REG_SETUP_AW) == 0x03);
}

void nrf24_set_channel(uint8_t channel) {
    if(channel > 125) return; // Prevent invalid channel
    nrf24_write_register(NRF24_REG_RF_CH, channel);
}

void nrf24_write_payload(uint8_t* data, uint8_t length) {
    if(length > 32) return; // NRF24L01+ limit
    
    uint8_t cmd = NRF24_CMD_W_TX_PAYLOAD;
    furi_hal_gpio_write(NRF24_CS_PIN, false);
    furi_hal_spi_bus_tx(NRF24_HANDLE, &cmd, 1, 100);
    furi_hal_spi_bus_tx(NRF24_HANDLE, data, length, 100);
    furi_hal_gpio_write(NRF24_CS_PIN, true);

    // Pulse CE to trigger transmission
    furi_hal_gpio_write(NRF24_CE_PIN, true);
    furi_delay_us(15);
    furi_hal_gpio_write(NRF24_CE_PIN, false);
}

void nrf24_write_payload_noack(uint8_t* data, uint8_t length) {
    if(length > 32) return; // NRF24L01+ limit
    
    uint8_t cmd = NRF24_CMD_W_TX_PAYLOAD_NOACK;
    furi_hal_gpio_write(NRF24_CS_PIN, false);
    furi_hal_spi_bus_tx(NRF24_HANDLE, &cmd, 1, 100);
    furi_hal_spi_bus_tx(NRF24_HANDLE, data, length, 100);
    furi_hal_gpio_write(NRF24_CS_PIN, true);

    // Pulse CE to trigger transmission
    furi_hal_gpio_write(NRF24_CE_PIN, true);
    furi_delay_us(15);
    furi_hal_gpio_write(NRF24_CE_PIN, false);
}

void nrf24_set_tx_mode(void) {
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    config &= ~(1 << 0); // Clear PRIM_RX bit
    config |= (1 << 1);  // Set PWR_UP
    nrf24_write_register(NRF24_REG_CONFIG, config);
    furi_delay_ms(2); // Wait for power-up
}

void nrf24_set_rx_mode(void) {
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    config |= (1 << 0); // Set PRIM_RX bit
    config |= (1 << 1); // Set PWR_UP
    nrf24_write_register(NRF24_REG_CONFIG, config);
    furi_delay_ms(2); // Wait for power-up
    
    // Enable RX
    furi_hal_gpio_write(NRF24_CE_PIN, true);
}

void nrf24_flush_rx(void) {
    uint8_t cmd = NRF24_CMD_FLUSH_RX;
    
    furi_hal_gpio_write(NRF24_CS_PIN, false);
    furi_hal_spi_bus_tx(NRF24_HANDLE, &cmd, 1, 100);
    furi_hal_gpio_write(NRF24_CS_PIN, true);
}

void nrf24_set_power(uint8_t power) {
    if(power > 3) power = 3; // Limit to valid range
    
    // Read current RF setup
    uint8_t rf_setup = nrf24_read_register(NRF24_REG_RF_SETUP);
    
    // Clear power bits (bits 1-2)
    rf_setup &= ~(0x06);
    
    // Set new power level
    rf_setup |= (power << 1);
    
    // Write back to register
    nrf24_write_register(NRF24_REG_RF_SETUP, rf_setup);
}