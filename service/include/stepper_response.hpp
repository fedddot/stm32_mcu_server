#ifndef	STEPPER_RESPONSE_HPP
#define	STEPPER_RESPONSE_HPP

#include <optional>
#include <string>

#include "stepper_types.hpp"

namespace service {
	class StepperResponse {
	public:
		enum class Result: int {
			SUCCESS,
			FAILURE,
		};
		StepperResponse(
			const Result& result,
			const std::optional<std::string>& error_message = std::nullopt,
			const std::optional<State>& state = std::nullopt
		): m_result(result), m_error_message(error_message), m_state(state) {

		}
		StepperResponse(const StepperResponse&) = default;
		StepperResponse& operator=(const StepperResponse&) = delete;
		virtual ~StepperResponse() noexcept = default;

		Result result() const {
			return m_result;
		}
		std::optional<State> state() const {
			return m_state;
		}
	private:
		const Result m_result;
		const std::optional<std::string> m_error_message;
		const std::optional<State> m_state;
	};
}

#endif // STEPPER_RESPONSE_HPP