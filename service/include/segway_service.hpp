#ifndef	SEGWAY_SERVICE_HPP
#define	SEGWAY_SERVICE_HPP

#include <stdexcept>

#include "motor_controller.hpp"
#include "service.hpp"
#include "segway_request.hpp"
#include "segway_response.hpp"
#include "inertial_module_controller.hpp"

namespace service {

	class SegwayService: public Service<SegwayRequest, SegwayResponse> {
	public:
		SegwayService(
			provider::InertialModuleController *inertial_module_controller,
			provider::MotorController *motor_left,
			provider::MotorController *motor_right
		): m_inertial_module_controller(inertial_module_controller),
		   m_motor_left(motor_left),
		   m_motor_right(motor_right) {
			if (!m_inertial_module_controller || !m_motor_left || !m_motor_right) {
				throw std::invalid_argument("invalid arguments");
			}
		}
		SegwayResponse run_api_request(const SegwayRequest& request) override {
			throw std::runtime_error("not implemented");
		}
		void balance() {
			throw std::runtime_error("not implemented");
		}
	private:
		provider::InertialModuleController *m_inertial_module_controller;
		provider::MotorController *m_motor_left;
		provider::MotorController *m_motor_right;
	};
}

#endif // SEGWAY_SERVICE_HPP