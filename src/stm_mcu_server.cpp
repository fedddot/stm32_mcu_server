#include <cstdint>
#include <cstring>
#include <stdexcept>

extern "C" {
    int main(void);
    void Reset_Handler(void);
    extern std::uint32_t _data_start_ram;
    extern std::uint32_t _data_size;
    extern std::uint32_t _data_flash_addr;
}

typedef void (*isr_procedure_t)(void);

__attribute__((section(".isr_vector"), used)) std::uint32_t g_pfnVectors[68] = {
    0x20005000,
    (std::uint32_t)(&Reset_Handler),
};

void Reset_Handler(void) {
    const auto source_ptr = (const char *)_data_flash_addr;
    auto dest_ptr = (char *)_data_start_ram;
    auto size = _data_size;
    while (size) {
        *dest_ptr = *source_ptr;
        --size;
    }
    main();
}

int main(void) {
    const auto str_var = std::string("asdf");
    while (1) {
    }
}