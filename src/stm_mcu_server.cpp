#include <cstdint>

#include "ring_buffer_input_stream.hpp"
#include "stepper_service.hpp"

#include "stm32f103xb.h"

using namespace service;

#ifndef DATA_BUFFER_SIZE
#  error "DATA_BUFFER_SIZE is not defined"
#endif
#ifndef PACKAGE_HEADER_LENGTH
#  error "DATA_BUFFER_SIZE is not defined"
#endif

ipc::RingBufferInputStream<std::uint8_t, DATA_BUFFER_SIZE> s_buffer;

static void init_clock();

int main(void) {
    init_clock();
    StepperService service;
    while (true) {
        // app.run_once();
    }
}

void init_clock(void) {
    // 1. Enable HSI (internal 8 MHz RC oscillator)
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {
        // Wait for HSI ready
    }

    // 2. Set HSI as system clock
    RCC->CFGR &= ~RCC_CFGR_SW; // Clear SW bits
    RCC->CFGR |= RCC_CFGR_SW_HSI; // Select HSI as system clock

    // 3. Wait until HSI is used as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
        // Wait for switch
    }

    // 4. Set AHB, APB1, APB2 prescalers to /1 (no division)
    RCC->CFGR &= ~RCC_CFGR_HPRE;  // AHB prescaler = 1
    RCC->CFGR &= ~RCC_CFGR_PPRE1; // APB1 prescaler = 1
    RCC->CFGR &= ~RCC_CFGR_PPRE2; // APB2 prescaler = 1

    // 5. Disable PLL (not used)
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY) {
        // Wait for PLL to be disabled
    }

    // 6. Optionally, set Flash latency to 0 (for HSI 8MHz, 0 wait states)
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_0;
}