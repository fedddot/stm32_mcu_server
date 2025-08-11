#include <climits>
#include <cstdio>
#include <cstring>

#include "stm_display_controller.hpp"
#include "driver_ssd1306.h"
#include "stm32f103xb.h"

#define SSD1306_BASIC_DEFAULT_DESELECT_LEVEL                   SSD1306_DESELECT_LEVEL_0P77                /**< set deselect level 0.77 */
#define SSD1306_BASIC_DEFAULT_LEFT_RIGHT_REMAP                 SSD1306_LEFT_RIGHT_REMAP_DISABLE           /**< disable remap */
#define SSD1306_BASIC_DEFAULT_PIN_CONF                         SSD1306_PIN_CONF_ALTERNATIVE               /**< set alternative */
#define SSD1306_BASIC_DEFAULT_PHASE1_PERIOD                    0x01                                       /**< set phase 1 */
#define SSD1306_BASIC_DEFAULT_PHASE2_PERIOD                    0x0F                                       /**< set phase F */
#define SSD1306_BASIC_DEFAULT_OSCILLATOR_FREQUENCY             0x08                                       /**< set 8 */
#define SSD1306_BASIC_DEFAULT_CLOCK_DIVIDE                     0x00                                       /**< set clock div 0 */
#define SSD1306_BASIC_DEFAULT_DISPLAY_OFFSET                   0x00                                       /**< set display offset */
#define SSD1306_BASIC_DEFAULT_MULTIPLEX_RATIO                  0x3F                                       /**< set ratio */
#define SSD1306_BASIC_DEFAULT_DISPLAY_MODE                     SSD1306_DISPLAY_MODE_NORMAL                /**< set normal mode */
#define SSD1306_BASIC_DEFAULT_SCAN_DIRECTION                   SSD1306_SCAN_DIRECTION_COMN_1_START        /**< set scan 1 */
#define SSD1306_BASIC_DEFAULT_SEGMENT                          SSD1306_SEGMENT_COLUMN_ADDRESS_127         /**< set column 127 */
#define SSD1306_BASIC_DEFAULT_CONTRAST                         0xCF                                       /**< set contrast CF */
#define SSD1306_BASIC_DEFAULT_ZOOM_IN                          SSD1306_ZOOM_IN_DISABLE                    /**< disable zoom in */
#define SSD1306_BASIC_DEFAULT_FADE_BLINKING_MODE               SSD1306_FADE_BLINKING_MODE_DISABLE         /**< disable fade */
#define SSD1306_BASIC_DEFAULT_FADE_FRAMES                      0x00                                       /**< set frame 0 */
#define SSD1306_BASIC_DEFAULT_DISPLAY_START_LINE               0x00                                       /**< set start line 0 */
#define SSD1306_BASIC_DEFAULT_HIGH_COLUMN_START_ADDRESS        0x00                                       /**< set high start 0 */
#define SSD1306_BASIC_DEFAULT_LOW_COLUMN_START_ADDRESS         0x00                                       /**< set low start 0 */
#define SSD1306_BASIC_DEFAULT_PAGE_ADDRESS_RANGE_START         0x00                                       /**< set page range start */
#define SSD1306_BASIC_DEFAULT_PAGE_ADDRESS_RANGE_END           0x07                                       /**< set page range end */
#define SSD1306_BASIC_DEFAULT_COLUMN_ADDRESS_RANGE_START       0x00                                       /**< set range start */
#define SSD1306_BASIC_DEFAULT_COLUMN_ADDRESS_RANGE_END         0x7F                                       /**< set range end */

inline uint8_t iic_init(void) {
    // Enable GPIOB and I2C1 clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Configure PB6 (SCL) and PB7 (SDA) as alternate function open-drain
    // PB6: SCL, PB7: SDA
    GPIOB->CRL &= ~((GPIO_CRL_MODE6 | GPIO_CRL_CNF6) | (GPIO_CRL_MODE7 | GPIO_CRL_CNF7));
    GPIOB->CRL |= (GPIO_CRL_MODE6_1 | GPIO_CRL_MODE6_0 | GPIO_CRL_CNF6_1); // 50MHz, AF open-drain
    GPIOB->CRL |= (GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0 | GPIO_CRL_CNF7_1); // 50MHz, AF open-drain

    // Reset I2C1
    I2C1->CR1 = I2C_CR1_SWRST;
    I2C1->CR1 = 0;

    // Set peripheral clock frequency (PCLK1 in MHz)
    I2C1->CR2 = 8; // Assuming 8 MHz PCLK1

    // Set clock control register for 100kHz (standard mode)
    // CCR = Fpclk1 / (2 * Fi2c) = 8MHz / (2*100kHz) = 40
    I2C1->CCR = 40;

    // Set rise time register (max rise time = 1000ns for standard mode)
    // TRISE = Fpclk1 (MHz) + 1 = 9
    I2C1->TRISE = 9;

    // Enable I2C1
    I2C1->CR1 |= I2C_CR1_PE;

    return 0;
}

inline uint8_t iic_deinit(void) {
    // Disable I2C1
    I2C1->CR1 &= ~I2C_CR1_PE;

    // Optionally, reset I2C1 registers
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 = 0;

    // Optionally, disable I2C1 and GPIOB clocks
    RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN;
    // Do not disable GPIOB if other peripherals use it

    return 0;
}

inline uint8_t iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len) {
    // Wait until I2C1 is not busy
    while (I2C1->SR2 & I2C_SR2_BUSY) {}

    // 1. Generate START
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB)) {}
    I2C1->SR1 = 0; // Clear SB by reading SR1 then writing DR

    // 2. Send address (write)
    I2C1->DR = addr; // LSB=0 for write
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {}
    const auto temp = I2C1->SR1 | I2C1->SR2;
    (void)temp;
    // 3. Send register address
    while (!(I2C1->SR1 & I2C_SR1_TXE)) {}
    I2C1->DR = reg;
    while (!(I2C1->SR1 & I2C_SR1_TXE)) {}

    // 4. Send data bytes
    for (uint16_t i = 0; i < len; ++i) {
        I2C1->DR = buf[i];
        while (!(I2C1->SR1 & I2C_SR1_TXE)) {}
    }

    // 5. Wait for transfer finished
    while (!(I2C1->SR1 & I2C_SR1_BTF)) {}

    // 6. Generate STOP
    I2C1->CR1 |= I2C_CR1_STOP;

    return 0;
}
inline uint8_t spi_init(void) {
    return 0;
}
inline uint8_t spi_deinit(void){
    return 0;
}
inline uint8_t spi_write_cmd(uint8_t *, uint16_t){
    return 0;
}
inline uint8_t spi_cmd_data_gpio_init(void){
    return 0;
}
inline uint8_t spi_cmd_data_gpio_deinit(void){
    return 0;
}
inline uint8_t spi_cmd_data_gpio_write(uint8_t){
    return 0;
}
inline uint8_t reset_gpio_init(void) {
    return 0;
}
inline uint8_t reset_gpio_deinit(void) {
    return 0;
}
inline uint8_t reset_gpio_write(uint8_t) {
    return 0;
}
inline void debug_print(const char *const, ...) {
    return;
}
inline void delay_ms(uint32_t) {
    return;
}

uint8_t stm32::StmDisplayController::ssd1306_basic_init(ssd1306_handle_t *handle, ssd1306_interface_t interface, I2CAddr addr) {
    uint8_t res;
    
    /* link functions */
    DRIVER_SSD1306_LINK_INIT(handle, ssd1306_handle_t);
    DRIVER_SSD1306_LINK_IIC_INIT(handle, iic_init);
    DRIVER_SSD1306_LINK_IIC_DEINIT(handle, iic_deinit);
    DRIVER_SSD1306_LINK_IIC_WRITE(handle, iic_write);
    DRIVER_SSD1306_LINK_SPI_INIT(handle, spi_init);
    DRIVER_SSD1306_LINK_SPI_DEINIT(handle, spi_deinit);
    DRIVER_SSD1306_LINK_SPI_WRITE_COMMAND(handle, spi_write_cmd);
    DRIVER_SSD1306_LINK_SPI_COMMAND_DATA_GPIO_INIT(handle, spi_cmd_data_gpio_init);
    DRIVER_SSD1306_LINK_SPI_COMMAND_DATA_GPIO_DEINIT(handle, spi_cmd_data_gpio_deinit);
    DRIVER_SSD1306_LINK_SPI_COMMAND_DATA_GPIO_WRITE(handle, spi_cmd_data_gpio_write);
    DRIVER_SSD1306_LINK_RESET_GPIO_INIT(handle, reset_gpio_init);
    DRIVER_SSD1306_LINK_RESET_GPIO_DEINIT(handle, reset_gpio_deinit);
    DRIVER_SSD1306_LINK_RESET_GPIO_WRITE(handle, reset_gpio_write);
    DRIVER_SSD1306_LINK_DELAY_MS(handle, delay_ms);
    DRIVER_SSD1306_LINK_DEBUG_PRINT(handle, debug_print);
    
    /* set interface */
    res = ssd1306_set_interface(handle, interface);
    if (res != 0)
    {
        debug_print("ssd1306: set interface failed.\n");
        
        return 1;
    }
    
    /* set addr pin */
    res = ssd1306_set_addr_pin(handle, addr);
    if (res != 0)
    {
        debug_print("ssd1306: set addr failed.\n");
        
        return 1;
    }
    
    /* ssd1306 init */
    res = ssd1306_init(handle);
    if (res != 0)
    {
        debug_print("ssd1306: init failed.\n");
        
        return 1;
    }
    
    /* close display */
    res = ssd1306_set_display(handle, SSD1306_DISPLAY_OFF);
    if (res != 0)
    {
        debug_print("ssd1306: set display failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set column address range */
    res = ssd1306_set_column_address_range(handle, SSD1306_BASIC_DEFAULT_COLUMN_ADDRESS_RANGE_START, SSD1306_BASIC_DEFAULT_COLUMN_ADDRESS_RANGE_END);
    if (res != 0)
    {
        debug_print("ssd1306: set column address range failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set page address range */
    res = ssd1306_set_page_address_range(handle, SSD1306_BASIC_DEFAULT_PAGE_ADDRESS_RANGE_START, SSD1306_BASIC_DEFAULT_PAGE_ADDRESS_RANGE_END);
    if (res != 0)
    {
        debug_print("ssd1306: set page address range failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set low column start address */
    res = ssd1306_set_low_column_start_address(handle, SSD1306_BASIC_DEFAULT_LOW_COLUMN_START_ADDRESS);
    if (res != 0)
    {
        debug_print("ssd1306: set low column start address failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set high column start address */
    res = ssd1306_set_high_column_start_address(handle, SSD1306_BASIC_DEFAULT_HIGH_COLUMN_START_ADDRESS);
    if (res != 0)
    {
        debug_print("ssd1306: set high column start address failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set display start line */
    res = ssd1306_set_display_start_line(handle, SSD1306_BASIC_DEFAULT_DISPLAY_START_LINE);
    if (res != 0)
    {
        debug_print("ssd1306: set display start line failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set fade blinking mode */
    res = ssd1306_set_fade_blinking_mode(handle, SSD1306_BASIC_DEFAULT_FADE_BLINKING_MODE, SSD1306_BASIC_DEFAULT_FADE_FRAMES);
    if (res != 0)
    {
        debug_print("ssd1306: set fade blinking failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* deactivate scroll */
    res = ssd1306_deactivate_scroll(handle);
    if (res != 0)
    {
        debug_print("ssd1306: set deactivate scroll failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set zoom in */
    res = ssd1306_set_zoom_in(handle, SSD1306_BASIC_DEFAULT_ZOOM_IN);
    if (res != 0)
    {
        debug_print("ssd1306: set set zoom in failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set contrast */
    res = ssd1306_set_contrast(handle, SSD1306_BASIC_DEFAULT_CONTRAST);
    if (res != 0)
    {
        debug_print("ssd1306: set contrast failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set segment remap */
    res = ssd1306_set_segment_remap(handle, SSD1306_BASIC_DEFAULT_SEGMENT);
    if (res != 0)
    {
        debug_print("ssd1306: set segment remap failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set scan direction */
    res = ssd1306_set_scan_direction(handle, SSD1306_BASIC_DEFAULT_SCAN_DIRECTION);
    if (res != 0)
    {
        debug_print("ssd1306: set scan direction failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set display mode */
    res = ssd1306_set_display_mode(handle, SSD1306_BASIC_DEFAULT_DISPLAY_MODE);
    if (res != 0)
    {
        debug_print("ssd1306: set display mode failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set multiplex ratio */
    res = ssd1306_set_multiplex_ratio(handle, SSD1306_BASIC_DEFAULT_MULTIPLEX_RATIO);
    if (res != 0)
    {
        debug_print("ssd1306: set multiplex ratio failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set display offset */
    res = ssd1306_set_display_offset(handle, SSD1306_BASIC_DEFAULT_DISPLAY_OFFSET);
    if (res != 0)
    {
        debug_print("ssd1306: set display offset failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set display clock */
    res = ssd1306_set_display_clock(handle, SSD1306_BASIC_DEFAULT_OSCILLATOR_FREQUENCY, SSD1306_BASIC_DEFAULT_CLOCK_DIVIDE);
    if (res != 0)
    {
        debug_print("ssd1306: set display clock failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set pre charge period */
    res = ssd1306_set_precharge_period(handle, SSD1306_BASIC_DEFAULT_PHASE1_PERIOD, SSD1306_BASIC_DEFAULT_PHASE2_PERIOD);
    if (res != 0)
    {
        debug_print("ssd1306: set pre charge period failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set hardware pins conf */
    res = ssd1306_set_com_pins_hardware_conf(handle, SSD1306_BASIC_DEFAULT_PIN_CONF, SSD1306_BASIC_DEFAULT_LEFT_RIGHT_REMAP);
    if (res != 0)
    {
        debug_print("ssd1306: set com pins hardware conf failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set deselect level 0.77 */
    res = ssd1306_set_deselect_level(handle, SSD1306_BASIC_DEFAULT_DESELECT_LEVEL);
    if (res != 0)
    {
        debug_print("ssd1306: set deselect level failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* set page memory addressing mode */
    res = ssd1306_set_memory_addressing_mode(handle, SSD1306_MEMORY_ADDRESSING_MODE_PAGE);
    if (res != 0)
    {
        debug_print("ssd1306: set memory addressing level failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* enable charge pump */
    res = ssd1306_set_charge_pump(handle, SSD1306_CHARGE_PUMP_ENABLE);
    if (res != 0)
    {
        debug_print("ssd1306: set charge pump failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* entire display off */
    res = ssd1306_set_entire_display(handle, SSD1306_ENTIRE_DISPLAY_OFF);
    if (res != 0)
    {
        debug_print("ssd1306: set entire display failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* enable display */
    res = ssd1306_set_display(handle, SSD1306_DISPLAY_ON);
    if (res != 0)
    {
        debug_print("ssd1306: set display failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    /* clear screen */
    res = ssd1306_clear(handle);
    if (res != 0)
    {
        debug_print("ssd1306: clear failed.\n");
        (void)ssd1306_deinit(handle);
        
        return 1;
    }
    
    return 0;
}