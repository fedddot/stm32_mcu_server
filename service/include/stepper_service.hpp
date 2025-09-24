#ifndef	STEPPER_SERVICE_HPP
#define	STEPPER_SERVICE_HPP

#include <stdexcept>

#include "service.hpp"
#include "stepper_request.hpp"
#include "stepper_response.hpp"
#include "stepper_provider.hpp"

namespace service {
	class StepperService: public Service<StepperRequest, StepperResponse> {
	public:
		StepperService(provider::StepperProvider *provider): m_provider(provider) {
			if (!m_provider) {
				throw std::invalid_argument("null provider ptr received");
			}
		}
		StepperResponse run_api_request(const StepperRequest& request) override {
			throw std::runtime_error("not implemented");
		}
	private:
		provider::StepperProvider *m_provider;
	};
}

#endif // STEPPER_SERVICE_HPP