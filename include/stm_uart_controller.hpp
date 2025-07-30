#ifndef STM_UART_CONTROLLER_HPP
#define STM_UART_CONTROLLER_HPP

#include <cstdint>
#include <stdexcept>
#include <vector>
#include "stm32f103x6.h"
#include "data_buffer.hpp"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_uart.h"
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
            UART_HandleTypeDef huart1;
            huart1.Instance = USART1;
            huart1.Init.BaudRate = 115200;
            huart1.Init.WordLength = UART_WORDLENGTH_8B;
            huart1.Init.StopBits = UART_STOPBITS_1;
            huart1.Init.Parity = UART_PARITY_NONE;
            huart1.Init.Mode = UART_MODE_TX_RX;
            huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
            huart1.Init.OverSampling = UART_OVERSAMPLING_16;
            if (HAL_UART_Init(&huart1) != HAL_OK) {
                throw std::runtime_error("Failed to initialize UART");
            }
            m_huart = huart1;
            init_uart1_isr(&StmUartController::uart_rx_interrupt_handler);
            NVIC_EnableIRQ(USART1_IRQn);

            s_data_buffer = data_buffer;
        }
        StmUartController(const StmUartController&) = delete;
        StmUartController& operator=(const StmUartController&) = delete;
        virtual ~StmUartController() noexcept {
            NVIC_DisableIRQ(USART1_IRQn);
            init_uart1_isr(nullptr);
            HAL_UART_DeInit(&m_huart);
            s_data_buffer = nullptr;
        }
        void write(const std::vector<std::uint8_t>& data) {
            for (auto byte : data) {
                // Wait until TXE (Transmit data register empty)
                // while (!(USART1->SR & USART_SR_TXE)) {}
                USART1->DR = byte;
            }
        }
    private:
        UART_HandleTypeDef m_huart;
        static host_tools::DataBuffer<std::uint8_t> *s_data_buffer;
        static void uart_rx_interrupt_handler() {
            if (USART1->SR & USART_SR_RXNE) {
                std::uint8_t byte = static_cast<std::uint8_t>(USART1->DR);
                if (s_data_buffer) {
                    s_data_buffer->push_back(byte);
                }
            }
        }
    };

    inline host_tools::DataBuffer<std::uint8_t> *StmUartController::s_data_buffer = nullptr;
}

#endif // STM_UART_CONTROLLER_HPP
