cmake_minimum_required(VERSION 3.22)

include(gcc-arm-none-eabi.cmake)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(HAL_DRIVERS_PATH $ENV{STM32_CUBE_F1_SRC_PATH}/Drivers)

# file(
#     GLOB HAL_SOURCES
#     LIST_DIRECTORIES FALSE
#     CONFIGURE_DEPENDS ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Src/*.c
# )
add_executable(
    stm_mcu_server
    ${CMAKE_CURRENT_LIST_DIR}/src/stm_mcu_server.cpp
    ${HAL_DRIVERS_PATH}/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
    ${HAL_DRIVERS_PATH}/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    # ${HAL_SOURCES}
)
target_include_directories(
    stm_mcu_server
    PRIVATE
    ${HAL_DRIVERS_PATH}/STM32F1xx_HAL_Driver/Inc
    ${HAL_DRIVERS_PATH}/CMSIS/Include
    ${HAL_DRIVERS_PATH}/CMSIS/Device/ST/STM32F1xx/Include
    ${CMAKE_CURRENT_LIST_DIR}/driver/Core/Inc
)
target_compile_definitions(
    stm_mcu_server
    PRIVATE
    -DMSG_PREAMBLE="MSG_PREAMBLE"
    -DIPC_BAUD=115200UL
    -DIPC_BUFF_SIZE=256UL
    -DSTM32F103x6
)