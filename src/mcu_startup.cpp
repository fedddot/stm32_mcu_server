#include <cstring>
#include <stdexcept>

extern "C" {
    // defined by the linker
    extern char _sidata;
    extern char _sdata;
    extern char _edata;
    extern char _sbss;
    extern char _ebss;
    extern char __libc_init_array; // at libc
    void Reset_Handler(void);
}

extern int main(void); // at stm_mcu_server.cpp

void Reset_Handler(void) {
    auto flash_source_ptr = (const char *)&_sidata;
    auto ram_start_ptr = (char *)&_sdata;
    auto ram_end_ptr = (const char *)&_edata;
    auto size = ram_end_ptr - ram_start_ptr;
    if (nullptr == std::memcpy(ram_start_ptr, flash_source_ptr, size)) {
        throw std::runtime_error("failed to init .data section");
    }
    auto bss_start_ptr = (char *)&_sbss;
    auto bss_end_ptr = (const char *)&_ebss;
    const auto bss_size = bss_end_ptr - bss_start_ptr;
    if (nullptr == std::memset(bss_start_ptr, 0, bss_size)) {
        throw std::runtime_error("failed to init .bss section");
    }
    auto init_array = (void (*)(void))&__libc_init_array;
    init_array();
    main();
}