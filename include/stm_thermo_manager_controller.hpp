#ifndef STM_THERMO_MANAGER_CONTROLLER_HPP
#define STM_THERMO_MANAGER_CONTROLLER_HPP

#include "stm_timer_controller.hpp"
#include "thermostat_controller.hpp"

namespace stm32 {
    class StmThermoManagerController : public service::ThermostatController {
    public:
        double read_temperature() const override {
            return 33.0;
        }
        void set_relay_state(const bool state) override {
            ;
        }
        TaskGuard *schedule_task(const Task& task, const std::size_t period_ms) override {
            m_timer_controller.schedule_task(task, period_ms);
            return new TimerTaskGuard(&m_timer_controller);
        }
    private:
        StmTimerController m_timer_controller;

        class TimerTaskGuard: public TaskGuard {
        public:
            TimerTaskGuard(StmTimerController *timer_controller_ptr) : m_timer_controller_ptr(timer_controller_ptr) {}
            void unschedule() override {
                m_timer_controller_ptr->unschedule_task();
            }
        private:
            StmTimerController *m_timer_controller_ptr;
        };
    };
}

#endif // STM_THERMO_MANAGER_CONTROLLER_HPP
