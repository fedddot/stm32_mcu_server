#include <cstdint>

extern "C" {
    int main(void);
    void Reset_Handler(void);
    extern std::uint32_t _estack;
    extern std::uint32_t __data_start;
}

typedef void (*isr_procedure_t)(void);

__attribute__((section(".isr_vector"), used)) isr_procedure_t g_pfnVectors[2] = {
    (isr_procedure_t)(&_estack),
    Reset_Handler,
};

void Reset_Handler(void) {
    main();
}

int main(void) {
    const std::uint32_t jajaja = 0xdeadbeef;
    const std::uint8_t *data_start_ptr = (std::uint8_t *)__data_start;
    while (1) {
    }
}