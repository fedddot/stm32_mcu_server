#ifndef STM_UART_CONTROLLER_HPP
#define STM_UART_CONTROLLER_HPP

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "data_buffer.hpp"

namespace stm32 {
    class StmUartController {
    public:
        StmUartController(host_tools::DataBuffer<std::uint8_t> *data_buffer) {
            if (s_data_buffer) {
                throw std::runtime_error("an instance of uart controller already exists");
            }
            if (!data_buffer) {
                throw std::invalid_argument("null data buffer ptr received");
            }
            // TODO: init UART peripherals
            // TODO: init an interrupt handler for UART RX which pushes data into the s_data_buffer
            s_data_buffer = data_buffer;
        }
        StmUartController(const StmUartController&) = delete;
        StmUartController& operator=(const StmUartController&) = delete;
        virtual ~StmUartController() noexcept {
            // TODO: uninit UART peripherals
            s_data_buffer = nullptr;
        }
        void write(const std::vector<std::uint8_t>& data) {
            // TODO: write the data into UART TX
        }
    private:
        static host_tools::DataBuffer<std::uint8_t> *s_data_buffer;
    };

    inline host_tools::DataBuffer<std::uint8_t> *StmUartController::s_data_buffer = nullptr;
}

#endif // STM_UART_CONTROLLER_HPP
