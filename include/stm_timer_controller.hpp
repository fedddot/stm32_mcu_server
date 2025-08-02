#ifndef STM_TIMER_CONTROLLER_HPP
#define STM_TIMER_CONTROLLER_HPP

#include <functional>

#include "stm32f103xb.h"
#include "stm_isr_vector.hpp"

namespace stm32 {
    class StmTimerController {
    public:
        using Task = std::function<void(void)>;
        StmTimerController() = default;
        StmTimerController(const StmTimerController&) = delete;
        StmTimerController& operator=(const StmTimerController&) = delete;
        virtual ~StmTimerController() = default;

        void schedule_task(const Task& task, const std::size_t& period_ms) {
            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
            init_timer1_up_isr(
                [task]() {
                    task();
                    TIM1->CNT = 0;
                    TIM1->SR &= ~TIM_SR_UIF; // Clear update interrupt flag
                }
            );
            TIM1->PSC = 7999; // Prescaler: 8 MHz / (7999+1) = 1 kHz (1 ms tick)
            TIM1->ARR = period_ms; // Auto-reload: period in ms
            TIM1->CNT = 0; // Clear counter
            TIM1->DIER |= TIM_DIER_UIE;
            TIM1->CR1 |= TIM_CR1_CEN;
            NVIC_EnableIRQ(TIM1_UP_IRQn);
        }
        void unschedule_task() {
            TIM1->CR1 &= ~TIM_CR1_CEN;
            TIM1->DIER &= ~TIM_DIER_UIE;
            init_timer1_up_isr(nullptr);
        }
    };
}

#endif // STM_TIMER_CONTROLLER_HPP
