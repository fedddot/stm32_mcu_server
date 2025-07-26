#include <climits>
#include <cstddef>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <vector>

#include "thermostat_host_builder.hpp"
#include "thermostat_controller.hpp"
#include "thermostat_service.hpp"
#include "raw_data_buffer.hpp"
#include "proto_thermostat_api_request_parser.hpp"

using namespace host;
using namespace service;

using RawData = RawDataBuffer<256UL>;
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

enum: std::size_t { ENCODED_SIZE_LEN = 4UL };
RawData s_buffer;
static ipc::ApiRequestParser s_api_request_parser;

int main(void) {
    StmThermoManagerController controller;
    ThermostatService service(&controller);

    auto host = create_host(&service);
    while (1) {
        host.run_once();
    }
}


static inline std::optional<std::size_t> retrieve_size(const RawData& data) {    
    if (data.size() < ENCODED_SIZE_LEN) {
        return std::nullopt;
    }
    std::size_t decoded_size(0UL);
    for (auto i = std::size_t(0UL); i < static_cast<std::size_t>(ENCODED_SIZE_LEN); ++i) {
        decoded_size <<= CHAR_BIT;
        decoded_size |= static_cast<std::size_t>(data.get(i));
    }
    return decoded_size;
}

static inline std::optional<std::vector<char>> read_payload(RawData *data, const std::size_t payload_size) {
    if (data->size() < ENCODED_SIZE_LEN + payload_size) {
        return std::nullopt;
    }
    std::size_t bytes_remaining = ENCODED_SIZE_LEN;
    while (bytes_remaining) {
        data->pop_first();
        --bytes_remaining;
    }
    std::vector<char> payload(payload_size);
    for (std::size_t i = 0; i < payload_size; ++i) {
        payload[i] = data->pop_first();
    }
    return payload;
}

inline Host<ApiRequest, ApiResponse> create_host(ThermoService *service_ptr) {
    
    return Host<ApiRequest, ApiResponse>(
        [](void) -> std::optional<ApiRequest> {
            enum: std::size_t { ENCODED_SIZE_LEN = 4UL };
            const auto payload_size = retrieve_size(s_buffer);
            if (!payload_size.has_value()) {
                return std::nullopt;
            }
            const auto payload = read_payload(&s_buffer, payload_size.value());
            if (!payload.has_value()) {
                return std::nullopt;
            }
            return s_api_request_parser(payload.value());
        },
        [](const ApiResponse& response) {
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