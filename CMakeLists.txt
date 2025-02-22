cmake_minimum_required(VERSION 3.12)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CC_COMPILER /usr/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/arm-none-eabi-g++)

add_executable(stm_mcu_server ${CMAKE_CURRENT_LIST_DIR}/src/stm_mcu_server.cpp)
target_include_directories(stm_mcu_server PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(
    stm_mcu_server
    PRIVATE
    host
    ipc_option
    protobuf_ipc_server

    stepper_motor
    stepper_motor_request
    stepper_motor_response
    stepper_motor_manager
    stepper_service

    pb_common
    pb_decode
    pb_encode

    stm_stdlib
)
target_compile_definitions(
    stm_mcu_server
    PRIVATE
    -DMSG_PREAMBLE="MSG_PREAMBLE"
    -DIPC_BAUD=115200UL
    -DIPC_BUFF_SIZE=256UL
)

add_subdirectory(external)

add_subdirectory(ipc)
add_subdirectory(platform)