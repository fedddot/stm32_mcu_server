#ifndef	SEGWAY_PROVIDER_HPP
#define	SEGWAY_PROVIDER_HPP

#include "provider.hpp"
#include "segway_types.hpp"

namespace provider {
	class SegwayProvider: public Provider {
	public:
		virtual ~SegwayProvider() noexcept = default;
		virtual void enable() = 0;
		virtual void disable() = 0;
		virtual void step(const int steps_number, const service::Direction direction, const service::Duration step_duration) = 0;
		virtual service::State status() const = 0;
	};
}

#endif // SEGWAY_PROVIDER_HPP