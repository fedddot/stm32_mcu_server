#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "ring_data_buffer.hpp"
#include "thermostat_host_builder.hpp"
#include "stm_thermo_manager_controller.hpp"
#include "stm_thermo_manager_controller.hpp"
#include "thermostat_service.hpp"
#include "proto_thermostat_api_request_parser.hpp"
#include "proto_thermostat_api_response_serializer.hpp"
#include "service_api.pb.h"
#include "stm_uart_controller.hpp"

using namespace host;
using namespace service;
using namespace host_tools;
using namespace stm32;

#define DATA_BUFFER_SIZE 256UL
#define PACKAGE_SIZE_FIELD_LENGTH 4UL

using HostBuilder = ThermostatHostBuilder<PACKAGE_SIZE_FIELD_LENGTH>;
using ApiRequest = HostBuilder::ApiRequest;
using ApiResponse = HostBuilder::ApiResponse;
using ThermoService = HostBuilder::Service;

static Host<ApiRequest, ApiResponse> create_host(ThermoService *service_ptr);
static void write_test_request(const ApiRequest& request);

RingDataBuffer<std::uint8_t, DATA_BUFFER_SIZE> s_buffer;
static ipc::ApiRequestParser s_api_request_parser;

int main(void) {
    StmThermoManagerController controller;
    ThermostatService service(&controller);
    StmUartController uart_controller(&s_buffer);

    const auto request = ApiRequest(ApiRequest::RequestType::GET_TEMP);
    write_test_request(request);

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
        .set_raw_data_writer(
            [](const std::vector<std::uint8_t>& data) {
                throw std::runtime_error("Raw data writer is not implemented");
            }
        )
        .set_service(service_ptr);
    return builder.build();
}

inline std::vector<std::uint8_t> serialize_thermostat_request(const ApiRequest& request) {
    const auto request_type_mapping = std::map<ThermostatApiRequest::RequestType, service_api_RequestType> {
        { ThermostatApiRequest::RequestType::START, service_api_RequestType_START },
        { ThermostatApiRequest::RequestType::STOP, service_api_RequestType_STOP },
        { ThermostatApiRequest::RequestType::GET_TEMP, service_api_RequestType_GET_TEMP }
    };
    auto casted_temp = float(0.0);
    if (request.temperature()) {
        casted_temp = static_cast<float>(request.temperature().value());
    }
    auto casted_time_resolution = std::size_t(0);
    if (request.time_resolution_ms()) {
        casted_time_resolution = request.time_resolution_ms().value();
    }
    const auto pb_request = service_api_ThermostatApiRequest {
        .request_type = request_type_mapping.at(request.type()),
        .set_temperature = casted_temp,
        .time_resolution_ms = static_cast<uint32_t>(casted_time_resolution),
    };
    pb_byte_t buffer[256];
    auto ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    if (!pb_encode(&ostream, service_api_ThermostatApiRequest_fields, &pb_request)) {
        throw std::runtime_error("Failed to encode ThermostatApiRequest to raw data");
    }
    return std::vector<std::uint8_t>((const char *)buffer, (const char *)(buffer + ostream.bytes_written));
}

inline void write_test_request(const ApiRequest& request) {
    const auto serial_data = serialize_thermostat_request(request);
    const auto data_size = serial_data.size();
    for (auto i = std::size_t(0); i < PACKAGE_SIZE_FIELD_LENGTH; ++i) {
        s_buffer.push_back((data_size >> (i * CHAR_BIT)) & 0xFF);
    }
    for (const auto byte : serial_data) {
        s_buffer.push_back(byte);
    }
}