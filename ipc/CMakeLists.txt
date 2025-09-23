set(CMAKE_MODULE_PATH $ENV{NANOPB_PATH}/extra)
find_package(Nanopb REQUIRED)

nanopb_generate_cpp(TARGET stepper_service_proto ${CMAKE_CURRENT_SOURCE_DIR}/proto/stepper_service.proto)

add_library(stepper_service_proto_generated INTERFACE)
target_sources(stepper_service_proto_generated INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/stepper_service.pb.c)
target_include_directories(stepper_service_proto_generated INTERFACE ${CMAKE_CURRENT_BINARY_DIR} $ENV{NANOPB_PATH})
add_dependencies(stepper_service_proto_generated stepper_service_proto)

add_library(pb_common INTERFACE)
target_sources(pb_common INTERFACE $ENV{NANOPB_PATH}/pb_common.c)
target_include_directories(pb_common INTERFACE $ENV{NANOPB_PATH})

add_library(pb_decode INTERFACE)
target_sources(pb_decode INTERFACE $ENV{NANOPB_PATH}/pb_decode.c)
target_include_directories(pb_decode INTERFACE $ENV{NANOPB_PATH})
target_link_libraries(pb_decode INTERFACE pb_common)

add_library(pb_encode INTERFACE)
target_sources(pb_encode INTERFACE $ENV{NANOPB_PATH}/pb_encode.c)
target_include_directories(pb_encode INTERFACE $ENV{NANOPB_PATH})
target_link_libraries(pb_encode INTERFACE pb_common)

add_library(pb_stepper_request_parser INTERFACE)
target_include_directories(pb_stepper_request_parser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(pb_stepper_request_parser INTERFACE stepper_service_proto_generated pb_decode)

add_library(pb_stepper_response_serializer INTERFACE)
target_include_directories(pb_stepper_response_serializer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(pb_stepper_response_serializer INTERFACE stepper_service_proto_generated pb_encode)

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