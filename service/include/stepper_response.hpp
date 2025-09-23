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
			const State& state,
			const std::optional<std::string>& error_message = std::nullopt
		): m_result(result), m_state(state), m_error_message(error_message) {

		}
		StepperResponse(const StepperResponse&) = default;
		StepperResponse& operator=(const StepperResponse&) = delete;
		virtual ~StepperResponse() noexcept = default;

		Result result() const {
			return m_result;
		}
		State state() const {
			return m_state;
		}
		std::optional<std::string> error_message() const {
			return m_error_message;
		}
	private:
		const Result m_result;
		const State m_state;
		const std::optional<std::string> m_error_message;
	};
}

#endif // STEPPER_RESPONSE_HPP