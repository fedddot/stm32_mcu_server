#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include "proto_thermostat_api_request_parser.hpp"
#include "proto_thermostat_api_response_serializer.hpp"

extern "C" {
    int main(void);
    void Reset_Handler(void);
    extern std::uint32_t g_stack_start;
    extern std::uint32_t g_data_start_ram;
    extern std::uint32_t g_data_size;
    extern std::uint32_t g_data_flash_addr;
    extern std::uint32_t g_tdata_start_ram;
    extern std::uint32_t g_tdata_end_ram;
    extern std::uint32_t g_tdata_flash_addr;
}

typedef void (*isr_procedure_t)(void);

__attribute__((section(".isr_vector"), used)) std::uint32_t g_pfnVectors[68] = {
    0x20005000,
    (std::uint32_t)(&Reset_Handler),
};

void Reset_Handler(void) {
    const auto source_ptr = (const void *)g_data_flash_addr;
    auto dest_ptr = (void *)g_data_start_ram;
    auto size = g_data_size;
    if (nullptr == std::memcpy(dest_ptr, source_ptr, size)) {
        throw std::runtime_error("failed to init .data section");
    }

    main();
}

int main(void) {
    const auto parser = ipc::ApiRequestParser();
    const auto serializer = ipc::ApiResponseSerializer();
    while (1) {
    }
}