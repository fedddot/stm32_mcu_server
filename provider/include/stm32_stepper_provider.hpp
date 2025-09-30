#ifndef	STM32_SEGWAY_PROVIDER_HPP
#define	STM32_SEGWAY_PROVIDER_HPP

#include <stdexcept>

#include "segway_provider.hpp"
#include "segway_types.hpp"

namespace provider {
	class Stm32SegwayProvider: public SegwayProvider {
	public:
		Stm32SegwayProvider() = default;
		Stm32SegwayProvider(const Stm32SegwayProvider&) = delete;
		Stm32SegwayProvider& operator=(const Stm32SegwayProvider&) = delete;

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

#endif // STM32_SEGWAY_PROVIDER_HPP