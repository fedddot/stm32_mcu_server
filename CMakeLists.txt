cmake_minimum_required(VERSION 3.22)

include(gcc-arm-none-eabi.cmake)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -frtti -fexceptions -fno-threadsafe-statics --specs=nosys.specs")

set(HAL_DRIVERS_PATH $ENV{STM32_CUBE_F1_SRC_PATH}/Drivers)
list(
    APPEND
    HAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/startup_stm32f103xb.s
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/Core/Src/system_stm32f1xx.c
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
    ${CMAKE_PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/src/${CMAKE_PROJECT_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/driver/startup_stm32f103xb.s
    ${HAL_SOURCES}
)
target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/driver/Core/Inc
    ${HAL_INCLUDE_DIRS}
)
target_compile_definitions(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    -DSTM32F103x6
)