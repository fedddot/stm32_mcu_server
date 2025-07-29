#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_flash.h"

#define VECTORS_TABLE_SIZE 0x42UL

extern "C" {
    // defined by the linker
    extern char _sidata;
    extern char _sdata;
    extern char _edata;
    extern char _estack;
    extern char _sbss;
    extern char _ebss;
    extern char __libc_init_array;
    extern void SystemInit(void);
    void reset_isr(void);
}

extern int main(void);
static void init_clock(void);

void reset_isr(void) {
    auto flash_source_ptr = (const char *)&_sidata;
    auto ram_start_ptr = (char *)&_sdata;
    auto ram_end_ptr = (const char *)&_edata;
    auto size = ram_end_ptr - ram_start_ptr;
    if (nullptr == std::memcpy(ram_start_ptr, flash_source_ptr, size)) {
        throw std::runtime_error("failed to init .data section");
    }
    auto bss_start_ptr = (char *)&_sbss;
    auto bss_end_ptr = (const char *)&_ebss;
    const auto bss_size = bss_end_ptr - bss_start_ptr;
    if (nullptr == std::memset(bss_start_ptr, 0, bss_size)) {
        throw std::runtime_error("failed to init .bss section");
    }
    SystemInit();
    auto init_array = (void (*)(void))&__libc_init_array;
    init_array();
    // init_clock();
    main();
}

void nmi_isr(void) {
    throw std::runtime_error("AHAHAHA");
    while (true) {
    }
}

void hard_fault_isr(void) {
    throw std::runtime_error("AHAHAHA");
    while (true) {
    }
}

__attribute__((section(".isr_vector"), used)) std::uint32_t g_pfnVectors[VECTORS_TABLE_SIZE] = {
    (std::uint32_t)(&_estack),
    (std::uint32_t)(&reset_isr),
    (std::uint32_t)(&nmi_isr),
    (std::uint32_t)(&hard_fault_isr),
};

void init_clock(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        hard_fault_isr();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        hard_fault_isr();
    }
}