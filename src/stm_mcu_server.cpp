extern "C" {
    int main(void);
    void Reset_Handler(void);
    extern unsigned long _estack;
}

typedef void (*isr_procedure_t)(void);

__attribute__((section(".isr_vector"), used)) isr_procedure_t g_pfnVectors[] = {
    (isr_procedure_t)(&_estack),
    Reset_Handler,
};

void Reset_Handler(void) {
    main();
}

int main(void) {
    while (1) {
    }
}