#ifndef	SEGWAY_RESPONSE_HPP
#define	SEGWAY_RESPONSE_HPP

#include <optional>
#include <string>

#include "segway_types.hpp"

namespace service {
	class SegwayResponse {
	public:
		enum class Result: int {
			SUCCESS,
			FAILURE,
		};
		SegwayResponse(
			const Result& result,
			const State& state,
			const std::optional<std::string>& error_message = std::nullopt
		): m_result(result), m_state(state), m_error_message(error_message) {

		}
		SegwayResponse(const SegwayResponse&) = default;
		SegwayResponse& operator=(const SegwayResponse&) = delete;
		virtual ~SegwayResponse() noexcept = default;

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

#endif // SEGWAY_RESPONSE_HPP