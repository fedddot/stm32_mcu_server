cmake_minimum_required(VERSION 3.22)

include(gcc-arm-none-eabi.cmake)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(HAL_DRIVER_PATH ${STM32_CUBE_F1_SRC_PATH}/Drivers/STM32F1xx_HAL_Driver)

add_executable(stm_mcu_server ${CMAKE_CURRENT_LIST_DIR}/src/stm_mcu_server.c)
target_include_directories(
    stm_mcu_server
    PRIVATE
    ${HAL_DRIVER_PATH}/Inc
)
target_compile_definitions(
    stm_mcu_server
    PRIVATE
    -DMSG_PREAMBLE="MSG_PREAMBLE"
    -DIPC_BAUD=115200UL
    -DIPC_BUFF_SIZE=256UL
)