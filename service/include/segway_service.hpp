#ifndef	SEGWAY_SERVICE_HPP
#define	SEGWAY_SERVICE_HPP

#include <stdexcept>

#include "service.hpp"
#include "segway_request.hpp"
#include "segway_response.hpp"
#include "segway_provider.hpp"

namespace service {
	class SegwayService: public Service<SegwayRequest, SegwayResponse> {
	public:
		SegwayService(provider::SegwayProvider *provider): m_provider(provider) {
			if (!m_provider) {
				throw std::invalid_argument("null provider ptr received");
			}
		}
		SegwayResponse run_api_request(const SegwayRequest& request) override {
			throw std::runtime_error("not implemented");
		}
	private:
		provider::SegwayProvider *m_provider;
	};
}

#endif // SEGWAY_SERVICE_HPP