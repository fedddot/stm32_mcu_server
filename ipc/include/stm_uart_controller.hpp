#ifndef STM_UART_CONTROLLER_HPP
#define STM_UART_CONTROLLER_HPP

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "stm32f103xb.h"

#include "stm_isr_vector.hpp"

#ifndef CLOCK_FREQUENCY
#  error "CLOCK_FREQUENCY is not defined"
#endif

namespace ipc {
    class StmUartController {
    public:
        using RxCallback = std::function<void(const std::uint8_t)>;
        StmUartController(const RxCallback& rx_callback) {
            if (s_rx_callback) {
                throw std::runtime_error("an instance of uart controller already exists");
            }
            if (!rx_callback) {
                throw std::invalid_argument("null rx_callback received");
            }
            init_uart();
            s_rx_callback = rx_callback;
        }
        StmUartController(const StmUartController&) = delete;
        StmUartController& operator=(const StmUartController&) = delete;
        virtual ~StmUartController() noexcept {
            uninit_uart();
            s_rx_callback = nullptr;
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
        static RxCallback s_rx_callback;
        static void uart_rx_interrupt_handler() {
            if (!s_rx_callback) {
                return;
            }
            if (USART1->SR & USART_SR_RXNE) {
                auto byte = USART1->DR;
                s_rx_callback(static_cast<std::uint8_t>(byte));
            }
            USART1->SR &= ~USART_SR_RXNE; // Clear RXNE flag
        }
        static void init_uart() {
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            stm32::init_uart1_isr(StmUartController::uart_rx_interrupt_handler);
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
            stm32::init_uart1_isr(nullptr);
            NVIC_DisableIRQ(USART1_IRQn);
        }
    };

    inline StmUartController::RxCallback StmUartController::s_rx_callback = nullptr;
}

#endif // STM_UART_CONTROLLER_HPP
