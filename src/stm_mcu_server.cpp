#include <cstring>
#include <optional>
#include <stdexcept>
#include <vector>

#include "thermostat_host_builder.hpp"
#include "thermostat_controller.hpp"
#include "thermostat_service.hpp"

using namespace host;
using namespace service;

using RawData = std::vector<char>;
using ApiRequest = ThermostatHostBuilder<RawData>::ApiRequest;
using ApiResponse = ThermostatHostBuilder<RawData>::ApiResponse;
using ThermoService = ThermostatHostBuilder<RawData>::Service;

class StmThermoManagerController: public ThermostatController {
public:
    double read_temperature() const override {
        return 33.0;
    }
    void set_relay_state(const bool state) override {
        ;
    }
    TaskGuard *schedule_task(const Task& task, const std::size_t period_ms) override {
        throw std::runtime_error("NOT IMPLEMENTED");
    }
};

static Host<ApiRequest, ApiResponse> create_host(ThermoService *service_ptr);

int main(void) {
    StmThermoManagerController controller;
    ThermostatService service(&controller);

    auto host = create_host(&service);
    while (1) {
        host.run_once();
    }
}

inline Host<ApiRequest, ApiResponse> create_host(ThermoService *service_ptr) {
    return Host<ApiRequest, ApiResponse>(
        [](void) -> std::optional<ApiRequest> {
            return std::nullopt;
        },
        [](const ApiResponse) {
            return;
        },
        [](const std::exception& e) -> ApiResponse {
            return ApiResponse(
                ApiResponse::Result::FAILURE,
                std::string(e.what())
            );
        },
        service_ptr
    );
}