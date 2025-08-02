#ifndef STM_TIMER_CONTROLLER_HPP
#define STM_TIMER_CONTROLLER_HPP

#include <functional>

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
            // TODO:
            // 1) init timer 1
            // 2) set the period for the timer
            // 3) set the callback to task_handler
            // 4) init timer isr
            
        }
        void unschedule_task(const Task& task) {
            // Implementation for unscheduling the task
            // This is a placeholder; actual implementation would depend on the system
        }
    private:
        static Task s_task;
        static void task_handler(void) {
            if (s_task) {
                s_task();
            }
        }
    };
}

#endif // STM_TIMER_CONTROLLER_HPP
