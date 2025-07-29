cmake_minimum_required(VERSION 3.22)

include(gcc-arm-none-eabi.cmake)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${CMAKE_CURRENT_LIST_DIR}/linker_script.ld\"")

add_library(mcu_startup INTERFACE)
target_sources(
    mcu_startup
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mcu_startup.cpp
    /usr/src/external/stm32_cube_f1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    /usr/src/external/stm32_cube_f1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    /usr/src/external/stm32_cube_f1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    /usr/src/external/stm32_cube_f1/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
)
target_include_directories(
    mcu_startup
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    $ENV{STM32_CUBE_SRC_PATH}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    $ENV{STM32_CUBE_SRC_PATH}/Drivers/STM32F1xx_HAL_Driver/Inc
    $ENV{STM32_CUBE_SRC_PATH}/Drivers/CMSIS/Include
)
target_link_libraries(
    mcu_startup
    INTERFACE
    c
)

add_library(stm_thermo_manager_controller INTERFACE)
target_include_directories(stm_thermo_manager_controller INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(
    stm_thermo_manager_controller
    INTERFACE
    thermostat_controller
)

add_library(stm_uart_controller INTERFACE)
target_include_directories(stm_uart_controller INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(
    stm_uart_controller
    INTERFACE
    data_buffer
)

add_executable(
    ${CMAKE_PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/src/${CMAKE_PROJECT_NAME}.cpp
)

target_compile_definitions(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    -DSTM32F103x6
)
target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    mcu_startup
    ring_data_buffer
    thermostat_host_builder
    thermostat_controller
    thermostat_service
    proto_thermostat_api_request_parser
    proto_thermostat_api_response_serializer
    stm_thermo_manager_controller
    stm_uart_controller
)

add_subdirectory($ENV{MCU_SERVER_PATH} ${CMAKE_BINARY_DIR}/_deps/mcu_server-build)