#ifndef	STEPPER_PROVIDER_HPP
#define	STEPPER_PROVIDER_HPP

#include "provider.hpp"
#include "stepper_types.hpp"

namespace provider {
	class StepperProvider: public Provider {
	public:
		virtual ~StepperProvider() noexcept = default;
		virtual void enable() = 0;
		virtual void disable() = 0;
		virtual void step(const int steps_number, const service::Direction direction, const service::Duration step_duration) = 0;
		virtual service::State status() const = 0;
	};
}

#endif // STEPPER_PROVIDER_HPP