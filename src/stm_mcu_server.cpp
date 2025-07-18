
// Forward declarations
extern "C" {
    int main(void);
    void Reset_Handler(void);
    extern unsigned long _estack; // Defined in linker script
}

typedef void (*isr_procedure_t)(void);

__attribute__((section(".isr_vector"), used)) isr_procedure_t g_pfnVectors[] = {
    (void (*)(void))(&_estack),   // Initial stack pointer
    Reset_Handler,                // Reset vector
};

void Reset_Handler(void) {
    main();
}

int main(void) {
    while (1) {
    }
}