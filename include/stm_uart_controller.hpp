#ifndef STM_UART_CONTROLLER_HPP
#define STM_UART_CONTROLLER_HPP

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "stm32f103xb.h"

#include "input_stream.hpp"
#include "stm_isr_vector.hpp"

#ifndef CLOCK_FREQUENCY
#  error "CLOCK_FREQUENCY is not defined"
#endif

namespace stm32 {
    class StmUartController {
    public:
        StmUartController(ipc::InputStream<std::uint8_t> *ipc_queue) {
            if (s_ipc_queue) {
                throw std::runtime_error("an instance of uart controller already exists");
            }
            if (!ipc_queue) {
                throw std::invalid_argument("null data buffer ptr received");
            }
            init_uart();
            s_ipc_queue = ipc_queue;
        }
        StmUartController(const StmUartController&) = delete;
        StmUartController& operator=(const StmUartController&) = delete;
        virtual ~StmUartController() noexcept {
            uninit_uart();
            s_ipc_queue = nullptr;
        }
        void write(const std::vector<std::uint8_t>& data) {
            for (const auto byte : data) {
                while (!(USART1->SR & USART_SR_TXE)) {
                    ;
                }
                USART1->DR = static_cast<uint8_t>(byte);
            }
            while (!(USART1->SR & USART_SR_TC)) {
                ;
            }
        }
    private:
        static ipc::InputStream<std::uint8_t> *s_ipc_queue;
        static void uart_rx_interrupt_handler() {
            if (!s_ipc_queue) {
                return;
            }
            if (USART1->SR & USART_SR_RXNE) {
                auto byte = USART1->DR;
                s_ipc_queue->enqueue(static_cast<std::uint8_t>(byte));
            }
            USART1->SR &= ~USART_SR_RXNE; // Clear RXNE flag
        }
        static void init_uart() {
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            init_uart1_isr(StmUartController::uart_rx_interrupt_handler);
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
            GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
            GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0);
            GPIOA->CRH |= GPIO_CRH_CNF9_1;
            GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
            GPIOA->CRH |= GPIO_CRH_CNF10_0;
            USART1->CR1 = 0;
            USART1->BRR = CLOCK_FREQUENCY / 115200UL;
            USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
            USART1->CR1 |= USART_CR1_RXNEIE;
            USART1->CR2 = 0;
            USART1->CR3 = 0;
            USART1->CR1 |= USART_CR1_UE;
            NVIC_EnableIRQ(USART1_IRQn);
        }
        static void uninit_uart() {
            USART1->CR1 &= ~USART_CR1_UE;
            init_uart1_isr(nullptr);
            NVIC_DisableIRQ(USART1_IRQn);
        }
    };

    inline ipc::InputStream<std::uint8_t> *StmUartController::s_ipc_queue = nullptr;
}

#endif // STM_UART_CONTROLLER_HPP
