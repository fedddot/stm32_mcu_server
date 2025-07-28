#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string_view>

#include "ring_data_buffer.hpp"
#include "thermostat_host_builder.hpp"
#include "thermostat_controller.hpp"
#include "thermostat_service.hpp"
#include "proto_thermostat_api_request_parser.hpp"
#include "proto_thermostat_api_response_serializer.hpp"

using namespace host;
using namespace service;
using namespace host_tools;

#define DATA_BUFFER_SIZE 256UL
#define PACKAGE_SIZE_FIELD_LENGTH 4UL

using HostBuilder = ThermostatHostBuilder<PACKAGE_SIZE_FIELD_LENGTH>;
using ApiRequest = HostBuilder::ApiRequest;
using ApiResponse = HostBuilder::ApiResponse;
using ThermoService = HostBuilder::Service;

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

RingDataBuffer<std::uint8_t, DATA_BUFFER_SIZE> s_buffer;
static ipc::ApiRequestParser s_api_request_parser;

int main(void) {
    StmThermoManagerController controller;
    ThermostatService service(&controller);

    const std::string_view test_message = "test_message";
    char encoded_size[] =  { 0, 0, 0, static_cast<char>(test_message.size()) };
    for (const auto ch : encoded_size) {
        s_buffer.push_back(ch);
    }
    for (const auto ch : test_message) {
        s_buffer.push_back(ch);
    }
    auto host = create_host(&service);
    while (1) {
        host.run_once();
    }
}

inline Host<ApiRequest, ApiResponse> create_host(ThermoService *service_ptr) {
    HostBuilder builder;
    builder
        .set_api_request_parser(ipc::ApiRequestParser())
        .set_api_response_serializer(ipc::ApiResponseSerializer())
        .set_raw_data_buffer(&s_buffer)
        .set_service(service_ptr);
    return builder.build();
}