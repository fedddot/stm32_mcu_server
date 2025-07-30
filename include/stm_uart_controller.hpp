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
            if (!s_data_buffer) {
                return;
            }
            if (USART1->SR & USART_SR_RXNE) {
                auto byte = USART1->DR;
                s_data_buffer->push_back(static_cast<std::uint8_t>(byte));
            }
        }
        static void init_uart() {
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
            GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
            GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0);
            GPIOA->CRH |= GPIO_CRH_CNF9_1;
            GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
            GPIOA->CRH |= GPIO_CRH_CNF10_0;
            USART1->CR1 = 0;
            USART1->BRR = 8000000UL / 115200UL;
            USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
            USART1->CR1 |= USART_CR1_RXNEIE;
            USART1->CR2 = 0;
            USART1->CR3 = 0;
            USART1->CR1 |= USART_CR1_UE;
            init_uart1_isr(StmUartController::uart_rx_interrupt_handler);
            NVIC_EnableIRQ(USART1_IRQn);
        }
        static void uninit_uart() {
            USART1->CR1 &= ~USART_CR1_UE;
            init_uart1_isr(nullptr);
            NVIC_DisableIRQ(USART1_IRQn);
        }
    };

    inline host_tools::DataBuffer<std::uint8_t> *StmUartController::s_data_buffer = nullptr;
}

#endif // STM_UART_CONTROLLER_HPP
