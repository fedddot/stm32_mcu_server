#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "stm_thermo_manager_controller.hpp"
#include "stm_uart_controller.hpp"

#include "ipc_queue.hpp"
#include "package_utils.hpp"
#include "proto_thermostat_api_request_parser.hpp"
#include "proto_thermostat_api_response_serializer.hpp"
#include "ring_queue.hpp"
#include "thermostat_app.hpp"

#include "stm32f103xb.h"

using namespace host;
using namespace service;
using namespace stm32;

#ifndef DATA_BUFFER_SIZE
#  error "DATA_BUFFER_SIZE is not defined"
#endif
#ifndef PACKAGE_HEADER_LENGTH
#  error "DATA_BUFFER_SIZE is not defined"
#endif

using App = ThermostatApp<PACKAGE_HEADER_LENGTH>;
using ApiRequest = App::ApiRequest;
using ApiResponse = App::ApiResponse;

ipc::RingQueue<std::uint8_t, DATA_BUFFER_SIZE> s_buffer;
static ipc::ApiRequestParser s_api_request_parser;

static void init_clock();

int main(void) {
    init_clock();
    StmUartController uart_controller(&s_buffer);
    StmThermoManagerController controller;
    
	const auto package_size_retriever = [](const ipc::IpcQueue<std::uint8_t>& package_size_data) -> std::size_t {
		std::vector<std::uint8_t> package_size_data_vector(PACKAGE_HEADER_LENGTH, 0);
		for (std::size_t i = 0; i < PACKAGE_HEADER_LENGTH; ++i) {
			package_size_data_vector[i] = package_size_data.inspect(i);
		}
		return ipc::parse_package_size(package_size_data_vector);
	};
	const auto header_generator = [](const std::vector<std::uint8_t>& payload, const std::size_t& header_size) -> std::vector<std::uint8_t> {
		return ipc::serialize_package_size(payload.size(), header_size);
	};
	const auto raw_data_writer = [uart_controller_ptr = &uart_controller](const std::vector<std::uint8_t>& raw_data)  {
		uart_controller_ptr->write(raw_data);
	};
	const auto api_request_parser = ipc::ApiRequestParser();
	const auto api_response_serializer = ipc::ApiResponseSerializer();
	ThermostatApp<PACKAGE_HEADER_LENGTH> app(
		package_size_retriever,
		api_request_parser,
		api_response_serializer,
		header_generator,
		raw_data_writer,
		&s_buffer,
		&controller
	);

    while (true) {
        app.run_once();
    }
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

void init_clock(void) {
    // 1. Enable HSI (internal 8 MHz RC oscillator)
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {
        // Wait for HSI ready
    }

    // 2. Set HSI as system clock
    RCC->CFGR &= ~RCC_CFGR_SW; // Clear SW bits
    RCC->CFGR |= RCC_CFGR_SW_HSI; // Select HSI as system clock

    // 3. Wait until HSI is used as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
        // Wait for switch
    }

    // 4. Set AHB, APB1, APB2 prescalers to /1 (no division)
    RCC->CFGR &= ~RCC_CFGR_HPRE;  // AHB prescaler = 1
    RCC->CFGR &= ~RCC_CFGR_PPRE1; // APB1 prescaler = 1
    RCC->CFGR &= ~RCC_CFGR_PPRE2; // APB2 prescaler = 1

    // 5. Disable PLL (not used)
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY) {
        // Wait for PLL to be disabled
    }

    // 6. Optionally, set Flash latency to 0 (for HSI 8MHz, 0 wait states)
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_0;
}