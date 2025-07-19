#include <cstddef>
#include <cstdint>

extern "C" {
    int main(void);
    void Reset_Handler(void);
    extern std::uint32_t _estack;
    extern volatile std::uint32_t __data_start;
    extern std::uint32_t end;
    extern std::uint32_t __rodata_start;
}

typedef void (*isr_procedure_t)(void);

__attribute__((section(".isr_vector"), used)) isr_procedure_t g_pfnVectors[2] = {
    *(isr_procedure_t *)(&_estack),
    Reset_Handler,
};

void Reset_Handler(void) {
    main();
}

int main(void) {
    while (1) {
    }
}