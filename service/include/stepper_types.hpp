#ifndef	STEPPER_TYPES_HPP
#define	STEPPER_TYPES_HPP

#include <chrono>

namespace service {
	using Duration = std::chrono::duration<int>;
	enum class Direction: int {
		CLOCKWISE,
		COUNTERCLOCKWISE,
	};
	enum class State: int {
		ENABLED,
		DISABLED,
	};
}

#endif // STEPPER_TYPES_HPP