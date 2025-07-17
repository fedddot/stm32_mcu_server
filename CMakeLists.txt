cmake_minimum_required(VERSION 3.22)

include(gcc-arm-none-eabi.cmake)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(HAL_DRIVERS_PATH $ENV{STM32_CUBE_F1_SRC_PATH}/Drivers)

list(
    APPEND
    HAL_SOURCES
    ${HAL_DRIVERS_PATH}/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
    ${HAL_DRIVERS_PATH}/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
)
list(
    APPEND
    HAL_INCLUDE_DIRS
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Inc
    ${HAL_DRIVERS_PATH}/CMSIS/Include
    ${HAL_DRIVERS_PATH}/CMSIS/Device/ST/STM32F1xx/Include
)

add_executable(
    stm_mcu_server
    ${CMAKE_CURRENT_LIST_DIR}/src/stm_mcu_server.cpp
    ${HAL_SOURCES}
)
target_include_directories(
    stm_mcu_server
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/driver/Core/Inc
    ${HAL_INCLUDE_DIRS}
)
target_compile_definitions(
    stm_mcu_server
    PRIVATE
    -DSTM32F103x6
)