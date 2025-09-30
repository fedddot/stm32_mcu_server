#ifndef	MOTOR_CONTROLLER_HPP
#define	MOTOR_CONTROLLER_HPP

namespace provider {
	class MotorController {
	public:
		virtual ~MotorController() = default;
		virtual void apply(const double moment) = 0;
		virtual void release() = 0;
	};
}

#endif // MOTOR_CONTROLLER_HPP