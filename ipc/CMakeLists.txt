set(CMAKE_MODULE_PATH $ENV{NANOPB_PATH}/extra)
find_package(Nanopb REQUIRED)

nanopb_generate_cpp(TARGET stepper_service_proto ${CMAKE_CURRENT_SOURCE_DIR}/proto/stepper_service.proto)

add_library(pb_stepper_request_parser INTERFACE)
target_include_directories(
    pb_stepper_request_parser
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/ipc
    $ENV{NANOPB_PATH}
)
target_link_libraries(pb_stepper_request_parser INTERFACE stepper_service_proto)

add_library(pb_stepper_response_serializer INTERFACE)
target_include_directories(
    pb_stepper_response_serializer
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/ipc
    $ENV{NANOPB_PATH}
)
target_link_libraries(pb_stepper_response_serializer INTERFACE stepper_service_proto)

add_library(stm_uart_controller INTERFACE)
target_include_directories(
    stm_uart_controller
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    $ENV{STM32_CUBE_SRC_PATH}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
)
target_link_libraries(
    stm_uart_controller
    INTERFACE
    stm_isr_vector
)