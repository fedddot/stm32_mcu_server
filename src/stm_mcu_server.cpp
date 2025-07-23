#include <cstring>
#include <optional>
#include <stdexcept>

#include "ipc_data.hpp"
#include "ipc_data_writer.hpp"
#include "ipc_instance.hpp"
#include "thermostat_manager_controller.hpp"
#include "thermostat_api_request.hpp"
#include "thermostat_api_response.hpp"
#include "thermostat_host_builder.hpp"
#include "proto_thermostat_api_request_parser.hpp"
#include "proto_thermostat_api_response_serializer.hpp"

using namespace host;
using namespace vendor;
using namespace ipc;
using namespace manager;

static Host<ThermostatVendorApiRequest, ThermostatVendorApiResponse> create_host(ThermostatManagerController *controller_ptr);
class StmThermoManagerController: public ThermostatManagerController {
public:
    double read_temperature() const override {
        throw std::runtime_error("NOT IMPLEMENTED");
    }
    void set_relay_state(const bool state) override {
        throw std::runtime_error("NOT IMPLEMENTED");
    }
    manager::Instance<TaskGuard> schedule_task(const Task& task, const std::size_t period_ms) override {
        throw std::runtime_error("NOT IMPLEMENTED");
    }
};

int main(void) {
    StmThermoManagerController controller;
    auto host = create_host(&controller);
    while (1) {
        // host.run_once();
    }
}

class RawDataReader: public IpcDataReader<RawData> {
public:
    std::optional<ipc::Instance<RawData>> read() override {
        return std::nullopt;
    }
};

class RawDataWriter: public IpcDataWriter<RawData> {
public:
    void write(const RawData& data) const override {
        ;
    }
};

inline Host<ThermostatVendorApiRequest, ThermostatVendorApiResponse> create_host(ThermostatManagerController *controller_ptr) {
    ThermostatHostBuilder builder;
    builder
        .set_api_request_parser(ApiRequestParser())
        .set_api_response_serializer(ApiResponseSerializer())
        .set_raw_data_reader(ThermostatHostBuilder::RawDataReaderInstance(new RawDataReader()))
        .set_raw_data_writer(ThermostatHostBuilder::RawDataWriterInstance(new RawDataWriter()))
        .set_controller(controller_ptr);
    return builder.build();
}