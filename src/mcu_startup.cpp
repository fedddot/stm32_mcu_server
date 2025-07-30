#include <cstring>
#include <stdexcept>

#include "stm32f1xx_hal_gpio.h"
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
static void MX_GPIO_Init(void);

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
    init_clock();
    MX_GPIO_Init();
    main();
}

void init_clock(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        throw std::runtime_error("Failed to configure RCC Oscillators");
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        throw std::runtime_error("Failed to configure RCC Oscillators");
    }
}

void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}