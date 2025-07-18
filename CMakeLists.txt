cmake_minimum_required(VERSION 3.22)

include(gcc-arm-none-eabi.cmake)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
    ${CMAKE_PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/src/${CMAKE_PROJECT_NAME}.cpp
)
target_compile_definitions(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    -DSTM32F103x6
)