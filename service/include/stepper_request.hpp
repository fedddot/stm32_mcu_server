#ifndef	STEPPER_REQUEST_HPP
#define	STEPPER_REQUEST_HPP

#include <optional>

#include "stepper_types.hpp"

namespace service {
	class StepperRequest {
	public:
		enum class Type: int {
			ENABLE,
			DISABLE,
			STEPS,
			STATUS,
		};
		StepperRequest(
			const Type& type,
			const std::optional<Direction>& direction = std::nullopt,
			const std::optional<int>& steps_number = std::nullopt,
			const std::optional<Duration>& step_duration = std::nullopt
		): m_type(type), m_direction(direction), m_steps_number(steps_number), m_step_duration(step_duration) {

		}
		StepperRequest(const StepperRequest&) = default;
		StepperRequest& operator=(const StepperRequest&) = delete;
		virtual ~StepperRequest() noexcept = default;

		Type type() const {
			return m_type;
		}
		std::optional<Direction> direction() const {
			return m_direction;
		}
		std::optional<int> steps_number() const {
			return m_steps_number;
		}
		std::optional<Duration> step_duration() const {
			return m_step_duration;
		}
	private:
		const Type m_type;
		const std::optional<Direction> m_direction;
		const std::optional<int> m_steps_number;
		const std::optional<Duration> m_step_duration;
	};
}

#endif // STEPPER_REQUEST_HPP