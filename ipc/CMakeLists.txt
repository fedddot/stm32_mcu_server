add_library(stm_uart_controller INTERFACE)
target_sources(
    stm_uart_controller
    INTERFACE
)
target_include_directories(stm_uart_controller INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(
    stm_uart_controller
    INTERFACE
    input_stream
    stm_isr_vector
)