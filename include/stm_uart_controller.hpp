#ifndef STM_UART_CONTROLLER_HPP
#define STM_UART_CONTROLLER_HPP

#include <cstdint>
#include <stdexcept>
#include <vector>
#include "stm32f103x6.h"
#include "data_buffer.hpp"
#include "stm_isr_vector.hpp"

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
            init_uart();
            s_data_buffer = data_buffer;
        }
        StmUartController(const StmUartController&) = delete;
        StmUartController& operator=(const StmUartController&) = delete;
        virtual ~StmUartController() noexcept {
            uninit_uart();
            s_data_buffer = nullptr;
        }
        void write(const std::vector<std::uint8_t>& data) {
        }
    private:
        static host_tools::DataBuffer<std::uint8_t> *s_data_buffer;
        static void uart_rx_interrupt_handler() {
        }
        static void init_uart() {

        }
        static void uninit_uart() {
        }
    };

    inline host_tools::DataBuffer<std::uint8_t> *StmUartController::s_data_buffer = nullptr;
}

#endif // STM_UART_CONTROLLER_HPP
