#ifndef	SEGWAY_TYPES_HPP
#define	SEGWAY_TYPES_HPP

#include <chrono>

namespace service {
	using Duration = std::chrono::microseconds;
	enum class Direction: int {
		CLOCKWISE,
		COUNTERCLOCKWISE,
	};
	enum class State: int {
		ENABLED,
		DISABLED,
	};
}

#endif // SEGWAY_TYPES_HPP