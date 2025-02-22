cmake_minimum_required(VERSION 3.22)

project(stm_mcu_server C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CC_COMPILER /usr/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/arm-none-eabi-g++)

add_library(cube_mx_main INTERFACE)
target_sources(cube_mx_main INTERFACE ${CMAKE_CURRENT_LIST_DIR}/cube_mx_src/Src/main.c)
target_include_directories(
    cube_mx_main
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/cube_mx_src/Inc
)

add_executable(stm_mcu_server ${CMAKE_CURRENT_LIST_DIR}/src/stm_mcu_server.c)
target_include_directories(stm_mcu_server PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(
    stm_mcu_server
    PRIVATE
    cube_mx_main
)
target_compile_definitions(
    stm_mcu_server
    PRIVATE
    -DMSG_PREAMBLE="MSG_PREAMBLE"
    -DIPC_BAUD=115200UL
    -DIPC_BUFF_SIZE=256UL
)