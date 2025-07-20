#include <cstring>
#include <stdexcept>

#include "ipc_data.hpp"
#include "ipc_data_writer.hpp"
#include "ipc_instance.hpp"
#include "relay_controller.hpp"

#include "temperature_sensor_controller.hpp"
#include "thermostat_api_request.hpp"
#include "thermostat_api_response.hpp"
#include "thermostat_host_builder.hpp"
#include "timer_scheduler.hpp"
#include "proto_thermostat_api_request_parser.hpp"
#include "proto_thermostat_api_response_serializer.hpp"

using namespace host;
using namespace vendor;
using namespace ipc;
using namespace manager;

static Host<ThermostatVendorApiRequest, ThermostatVendorApiResponse> create_host(void);

int main(void) {
    auto host = create_host();
    while (1) {
        host.run_once();
    }
}

class RawDataReader: public IpcDataReader<RawData> {
public:
    std::optional<ipc::Instance<RawData>> read() override {
        return ipc::Instance<RawData>(new RawData());
    }
};

class RawDataWriter: public IpcDataWriter<RawData> {
public:
    void write(const RawData& data) const override {
        ;
    }
};

class StmRelayController: public RelayController {
public:
    void set_relay_state(const bool state) override {

    }
};

class StmTimerScheduler: public TimerScheduler {
public:
    manager::Instance<TaskGuard> schedule_task(const Task& task, const std::size_t period_ms) override {
        throw std::runtime_error("NOT IMPLEMENTED");
    }
};

class StmTempSensor: public TemperatureSensorController {
public:
    double read_temperature() const {
        return 29.0;
    }
};

inline Host<ThermostatVendorApiRequest, ThermostatVendorApiResponse> create_host(void) {
    ThermostatHostBuilder builder;
    builder
        .set_api_request_parser(ApiRequestParser())
        .set_api_response_serializer(ApiResponseSerializer())
        .set_raw_data_reader(ThermostatHostBuilder::RawDataReaderInstance(new RawDataReader()))
        .set_raw_data_writer(ThermostatHostBuilder::RawDataWriterInstance(new RawDataWriter()))
        .set_relay_controller(ThermostatHostBuilder::RelayControllerInstance(new StmRelayController()))
        .set_scheduler(ThermostatHostBuilder::SchedulerInstance(new StmTimerScheduler()))
        .set_temp_sensor(ThermostatHostBuilder::SensorInstance(new StmTempSensor()));
    return builder.build();
}