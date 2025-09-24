#ifndef	STM32_STEPPER_PROVIDER_HPP
#define	STM32_STEPPER_PROVIDER_HPP

#include "stepper_provider.hpp"
#include "stepper_types.hpp"
#include <stdexcept>

namespace provider {
	class Stm32StepperProvider: public StepperProvider {
	public:
		Stm32StepperProvider() = default;
		Stm32StepperProvider(const Stm32StepperProvider&) = delete;
		Stm32StepperProvider& operator=(const Stm32StepperProvider&) = delete;

		void enable() override {
			throw std::runtime_error("not implemented");
		}
		void disable() override {
			throw std::runtime_error("not implemented");
		}
		void step(const int steps_number, const service::Direction direction, const service::Duration step_duration) override {
			throw std::runtime_error("not implemented");
		}
		service::State status() const override {
			throw std::runtime_error("not implemented");
		}
	};
}

#endif // STM32_STEPPER_PROVIDER_HPP