#include <cstring>
#include <optional>
#include <stdexcept>

#include "host.hpp"
#include "ipc_data_writer.hpp"
#include "ipc_instance.hpp"
#include "thermostat_manager.hpp"
#include "thermostat_manager_controller.hpp"
#include "thermostat_api_request.hpp"
#include "thermostat_api_response.hpp"
#include "thermostat_vendor.hpp"

using namespace host;
using namespace vendor;
using namespace ipc;
using namespace manager;

static host::Host<ThermostatVendorApiRequest, ThermostatVendorApiResponse> create_host(ThermostatManagerController *controller_ptr);
class StmThermoManagerController: public ThermostatManagerController {
public:
    double read_temperature() const override {
        throw std::runtime_error("NOT IMPLEMENTED");
    }
    void set_relay_state(const bool state) override {
        ;
    }
    manager::Instance<TaskGuard> schedule_task(const Task& task, const std::size_t period_ms) override {
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
        return std::nullopt;
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