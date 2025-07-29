#ifndef STM_UART_CONTROLLER_HPP
#define STM_UART_CONTROLLER_HPP

#include <cstdint>
#include <stdexcept>
#include <vector>
#include "stm32f103x6.h"
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
            // Enable USART1 clock
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
            // Enable GPIOA clock (for PA9/PA10)
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

            // Configure PA9 (TX) as alternate function push-pull
            GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
            GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0); // Output mode, max speed 50 MHz
            GPIOA->CRH |= GPIO_CRH_CNF9_1; // AF push-pull

            // Configure PA10 (RX) as input floating
            GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
            GPIOA->CRH |= GPIO_CRH_CNF10_0;

            // Configure USART1: 115200 baud, 8N1
            USART1->BRR = SystemCoreClock / 115200;
            USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;

            // Enable USART1 interrupt in NVIC
            NVIC_SetVector(USART1_IRQn, (uint32_t)(&uart_rx_interrupt_handler));
            NVIC_EnableIRQ(USART1_IRQn);

            s_data_buffer = data_buffer;
        }
        StmUartController(const StmUartController&) = delete;
        StmUartController& operator=(const StmUartController&) = delete;
        virtual ~StmUartController() noexcept {
            // Disable USART1 interrupt in NVIC
            NVIC_DisableIRQ(USART1_IRQn);
            // Disable USART1
            USART1->CR1 &= ~USART_CR1_UE;
            s_data_buffer = nullptr;
        }
        void write(const std::vector<std::uint8_t>& data) {
            for (auto byte : data) {
                // Wait until TXE (Transmit data register empty)
                while (!(USART1->SR & USART_SR_TXE)) {}
                USART1->DR = byte;
            }
        }
    private:
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
