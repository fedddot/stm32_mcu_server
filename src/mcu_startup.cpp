#include <cstdint>
#include <cstring>
#include <stdexcept>

#define VECTORS_TABLE_SIZE 68UL

extern "C" {
    // defined by the linker
    extern char _sidata;
    extern char _sdata;
    extern char _edata;
    extern char _estack;
    
    void reset_isr(void);
}

extern int main(void);

void reset_isr(void) {
    auto flash_source_ptr = (const char *)&_sidata;
    auto ram_start_ptr = (char *)&_sdata;
    auto ram_end_ptr = (const char *)&_edata;
    auto size = ram_end_ptr - ram_start_ptr;
    if (nullptr == std::memcpy(ram_start_ptr, flash_source_ptr, size)) {
        throw std::runtime_error("failed to init .data section");
    }
    main();
}

void nmi_isr(void) {
    throw std::runtime_error("AHAHAHA");
    while (true) {
    }
}

void hard_fault_isr(void) {
    throw std::runtime_error("AHAHAHA");
    while (true) {
    }
}

__attribute__((section(".isr_vector"), used)) std::uint32_t g_pfnVectors[VECTORS_TABLE_SIZE] = {
    (std::uint32_t)(&_estack),
    (std::uint32_t)(&reset_isr),
    (std::uint32_t)(&nmi_isr),
    (std::uint32_t)(&hard_fault_isr),
};