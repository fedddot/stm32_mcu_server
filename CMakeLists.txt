cmake_minimum_required(VERSION 3.22)

include(gcc-arm-none-eabi.cmake)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"$ENV{STM32_CUBE_SRC_PATH}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/linker/STM32F103XB_FLASH.ld\"")

add_library(stm_isr_vector INTERFACE)
target_sources(stm_isr_vector INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/stm_isr_vector.cpp)
target_include_directories(stm_isr_vector INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(mcu_startup INTERFACE)
target_sources(
    mcu_startup
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mcu_startup.cpp
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
    stm_isr_vector
    c
)

add_library(stm_thermo_manager_controller INTERFACE)
target_include_directories(stm_thermo_manager_controller INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(
    stm_thermo_manager_controller
    INTERFACE
    thermostat_controller
    stm_uart_controller
)

add_library(stm_uart_controller INTERFACE)
target_sources(
    stm_uart_controller
    INTERFACE
)
target_include_directories(stm_uart_controller INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(
    stm_uart_controller
    INTERFACE
    ipc_queue
    stm_isr_vector
)

add_library(stm_timer_controller INTERFACE)
target_include_directories(
    stm_timer_controller
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    $ENV{STM32_CUBE_SRC_PATH}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
)
target_link_libraries(
    stm_timer_controller
    INTERFACE
    stm_isr_vector
)

add_library(ssd1306 STATIC)
target_sources(
    ssd1306
    PRIVATE
    $ENV{SSD1306_PATH}/src/driver_ssd1306.c
)
target_include_directories(
    ssd1306
    PUBLIC
    $ENV{SSD1306_PATH}/src
    $ENV{SSD1306_PATH}/interface
)

add_library(stm_display_controller INTERFACE)
target_sources(
    stm_display_controller
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stm_display_controller.cpp
)
target_include_directories(stm_display_controller INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(stm_display_controller INTERFACE ssd1306)

add_executable(
    ${CMAKE_PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/src/${CMAKE_PROJECT_NAME}.cpp
)

target_compile_definitions(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    -DSTM32F103x6
    -DDATA_BUFFER_SIZE=256UL
    -DPACKAGE_HEADER_LENGTH=4UL
    -DCLOCK_FREQUENCY=8000000UL
)
target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    mcu_startup

    stm_display_controller
    stm_thermo_manager_controller
    stm_uart_controller

    ipc_queue
    package_utils
    proto_thermostat_api_request_parser
    proto_thermostat_api_response_serializer
    ring_queue
    thermostat_app
)

add_subdirectory(embedded-service-framework ${CMAKE_BINARY_DIR}/_deps/embedded-service-framework-build)
add_subdirectory(service)