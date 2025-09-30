#ifndef	INERTIAL_MODULE_CONTROLLER_HPP
#define	INERTIAL_MODULE_CONTROLLER_HPP

#include "segway_types.hpp"

namespace provider {
	class InertialModuleController {
	public:
		virtual ~InertialModuleController() = default;
		virtual double read_rotation_speed(const service::Axis& axis) const = 0;
		virtual double read_acceleration(const service::Axis& axis) const = 0;
	};
}

#endif // INERTIAL_MODULE_CONTROLLER_HPP