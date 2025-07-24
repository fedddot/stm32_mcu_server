#include <cstring>
#include <optional>
#include <stdexcept>
#include <vector>

#include "thermostat_host_builder.hpp"
#include "thermostat_controller.hpp"

using namespace host;
using namespace service;

using RawData = std::vector<char>;
using ApiRequest = ThermostatHostBuilder<RawData>::ApiRequest;
using ApiResponse = ThermostatHostBuilder<RawData>::ApiResponse;

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

int main(void) {
    StmThermoManagerController controller;
    auto host = create_host(&controller);
    while (1) {
        host.run_once();
    }
}

class ApiRequestReader: public ipc::IpcDataReader<ThermostatVendorApiRequest> {
public:
    std::optional<ipc::Instance<ThermostatVendorApiRequest>> read() override {
        return ipc::Instance<ThermostatVendorApiRequest>(new ThermostatVendorApiRequest(ThermostatVendorApiRequest::RequestType::GET_TEMP));
    }
};

class ApiResposeWriter: public IpcDataWriter<ThermostatVendorApiResponse> {
public:
    void write(const ThermostatVendorApiResponse& data) const override {
        ;
    }
};

inline Host<ThermostatVendorApiRequest, ThermostatVendorApiResponse> create_host(ThermostatManagerController *controller_ptr) {
    using ThermoHost = Host<ThermostatVendorApiRequest, ThermostatVendorApiResponse>;
    const auto manager_instance = ThermostatVendor::ThermostatManagerInstance(
        new ThermostatManager(controller_ptr)
    );
    const auto vendor_instance = ThermoHost::VendorInstance(
        new ThermostatVendor(manager_instance)
    );
    return Host<ThermostatVendorApiRequest, ThermostatVendorApiResponse>(
        ThermoHost::ApiRequestReaderInstance(new ApiRequestReader()),
        ThermoHost::ApiResponseWriterInstance(new ApiResposeWriter()),
        vendor_instance,
        [](const std::exception& e) -> ThermostatVendorApiResponse {
            return ThermostatVendorApiResponse(
                ThermostatVendorApiResponse::Result::FAILURE,
                std::string(e.what())
            );
        }
    );
}