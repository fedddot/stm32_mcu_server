cmake_minimum_required(VERSION 3.22)

include(gcc-arm-none-eabi.cmake)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${CMAKE_CURRENT_LIST_DIR}/linker_script.ld\"")

add_executable(
    ${CMAKE_PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/src/${CMAKE_PROJECT_NAME}.cpp
)
target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    $ENV{STM32_CUBE_F1_SRC_PATH}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
)
target_compile_definitions(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    -DSTM32F103x6
)
target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ipc_data
    ipc_data_writer
    ipc_instance
    relay_controller

    temperature_sensor_controller
    thermostat_host_builder
    timer_scheduler
    proto_thermostat_api_request_parser
    proto_thermostat_api_response_serializer
)
add_subdirectory($ENV{MCU_SERVER_PATH} ${CMAKE_BINARY_DIR}/_deps/mcu_server-build)