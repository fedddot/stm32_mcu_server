#ifndef STM_DISPLAY_CONTROLLER_HPP
#define STM_DISPLAY_CONTROLLER_HPP

#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "driver_ssd1306.h"

namespace stm32 {
    class StmDisplayController {
    public:
        using I2CAddr = ssd1306_address_t;
        using Font = ssd1306_font_t;
        StmDisplayController(const I2CAddr& addr, const Font& font): m_font(font) {
            std::memset(&m_handle, 0, sizeof(ssd1306_handle_t));
            if (ssd1306_basic_init(&m_handle, ssd1306_interface_t::SSD1306_INTERFACE_IIC, addr)) {
                throw std::runtime_error("failed to init ssd1306 controller");
            }
        }
        StmDisplayController(const StmDisplayController&) = delete;
        StmDisplayController& operator=(const StmDisplayController&) = delete;
        virtual ~StmDisplayController() noexcept = default;
        void print(const std::string& what) const {
            if (ssd1306_gram_write_string(&m_handle, 0, 0, const_cast<char *>(what.c_str()), what.size(), 0xFF, m_font)) {
                throw std::runtime_error("failed to write message into ssd1306");
            }
            if (ssd1306_gram_update(&m_handle)) {
                throw std::runtime_error("failed to update ssd1306");
            }
        }
    private:
        mutable ssd1306_handle_t m_handle;
        const Font m_font;
        static uint8_t ssd1306_basic_init(ssd1306_handle_t *handle, ssd1306_interface_t interface, I2CAddr addr);
    };
}

#endif // STM_DISPLAY_CONTROLLER_HPP
