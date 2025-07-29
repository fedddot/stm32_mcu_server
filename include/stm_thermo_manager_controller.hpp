#ifndef STM_THERMO_MANAGER_CONTROLLER_HPP
#define STM_THERMO_MANAGER_CONTROLLER_HPP

#include <stdexcept>

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
            throw std::runtime_error("NOT IMPLEMENTED");
        }
    };
}

#endif // STM_THERMO_MANAGER_CONTROLLER_HPP
