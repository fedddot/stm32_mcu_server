#ifndef STM_ISR_VECTOR_HPP
#define STM_ISR_VECTOR_HPP

#include <functional>

namespace stm32 {
    using IsrCallback = std::function<void(void)>;

    void init_hard_fault_isr(const IsrCallback& callback);
    void init_nmi_isr(const IsrCallback& callback);
    void init_uart1_isr(const IsrCallback& callback);
}

#endif // STM_ISR_VECTOR_HPP
