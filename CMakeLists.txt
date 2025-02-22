cmake_minimum_required(VERSION 3.22)

include(mcu_hal/cmake/gcc-arm-none-eabi.cmake)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(mcu_hal/cmake/stm32cubemx)

add_executable(stm_mcu_server ${CMAKE_CURRENT_LIST_DIR}/src/stm_mcu_server.c)
target_include_directories(stm_mcu_server PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(
    stm_mcu_server
    PRIVATE
    stm32cubemx
)
target_compile_definitions(
    stm_mcu_server
    PRIVATE
    -DMSG_PREAMBLE="MSG_PREAMBLE"
    -DIPC_BAUD=115200UL
    -DIPC_BUFF_SIZE=256UL
)