#include <cstdint>
#include <cstring>

#include "stm_isr_vector.hpp"

#define VECTORS_TABLE_SIZE 0x42UL

extern "C" {
    extern char _estack;            // defined by the linker
}

extern void reset_isr(void);        // defined in mcu_startup.cpp

static stm32::IsrCallback s_hard_fault_isr_callback;
void stm32::init_hard_fault_isr(const IsrCallback& callback) {
    s_hard_fault_isr_callback = callback;
}
static void hard_fault_isr(void) {
    if (s_hard_fault_isr_callback) {
        s_hard_fault_isr_callback();
    }
}

static stm32::IsrCallback s_nmi_isr_callback;
void stm32::init_nmi_isr(const IsrCallback& callback) {
    s_nmi_isr_callback = callback;
}
static void nmi_isr(void) {
    if (s_nmi_isr_callback) {
        s_nmi_isr_callback();
    }
}

static stm32::IsrCallback s_uart1_isr_callback;
void stm32::init_uart1_isr(const IsrCallback& callback) {
    s_uart1_isr_callback = callback;
}
static void uart1_isr(void) {
    if (s_uart1_isr_callback) {
        s_uart1_isr_callback();
    }
}

__attribute__((section(".isr_vector"), used)) std::uint32_t g_pfnVectors[VECTORS_TABLE_SIZE] = {
    (std::uint32_t)(&_estack),
    (std::uint32_t)(&reset_isr),
    (std::uint32_t)(&nmi_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&uart1_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr),
    (std::uint32_t)(&hard_fault_isr)
};