#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string_view>

int main(void);
void Reset_Handler(void);

extern "C" {
    extern char _sidata;
    extern char _sdata;
    extern char _edata;
    extern char _estack;
}

typedef void (*isr_procedure_t)(void);

__attribute__((section(".isr_vector"), used)) std::uint32_t g_pfnVectors[68] = {
    (std::uint32_t)(&_estack),
    (std::uint32_t)(&Reset_Handler),
};

void Reset_Handler(void) {
    auto flash_source_ptr = (const char *)&_sidata;
    auto ram_start_ptr = (char *)&_sdata;
    auto ram_end_ptr = (const char *)&_edata;
    auto size = ram_end_ptr - ram_start_ptr;
    if (nullptr == std::memcpy(ram_start_ptr, flash_source_ptr, size)) {
        throw std::runtime_error("failed to init .data section");
    }
    main();
}

const auto g_str_var = std::string_view("asdfasdf");
auto g_int_var = std::uint32_t(0xdeadbeef);

int main(void) {
    (void)g_str_var;
    (void)g_int_var;
    while (1) {
    }
}