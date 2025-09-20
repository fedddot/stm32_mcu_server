#ifndef	STEPPER_SERVICE_HPP
#define	STEPPER_SERVICE_HPP

#include <stdexcept>

#include "service.hpp"
#include "stepper_request.hpp"
#include "stepper_response.hpp"

namespace service {
	class StepperService: public Service<StepperRequest, StepperResponse> {
	public:
		StepperResponse run_api_request(const StepperRequest& request) override {
			throw std::runtime_error("not implemented");
		}
	};
}

#endif // STEPPER_SERVICE_HPP