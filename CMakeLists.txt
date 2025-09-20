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
    api_message_reader
    api_message_writer
    host
    package_reader
    package_utils
    package_writer
    ring_buffer_input_stream
    stepper_request
    stepper_response
    stepper_service
    stm_uart_controller
)

add_subdirectory(embedded-service-framework ${CMAKE_BINARY_DIR}/_deps/embedded-service-framework-build)
add_subdirectory(service)
add_subdirectory(ipc)
add_subdirectory(provider)