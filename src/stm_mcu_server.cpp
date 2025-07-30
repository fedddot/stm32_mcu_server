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

#include "stm32f1xx_hal_conf.h"
#include "stm32f1xx_hal.h"

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

static Host<ApiRequest, ApiResponse> create_host(ThermoService *service_ptr, StmUartController *uart_controller_ptr);
static void write_test_request(const ApiRequest& request);

RingDataBuffer<std::uint8_t, DATA_BUFFER_SIZE> s_buffer;
static ipc::ApiRequestParser s_api_request_parser;

static void SystemClock_Config();
static void MX_GPIO_Init();

int main(void) {
    SystemClock_Config();
    MX_GPIO_Init();
    StmUartController uart_controller(&s_buffer);
    StmThermoManagerController controller;
    ThermostatService service(&controller);

    const auto request = ApiRequest(ApiRequest::RequestType::GET_TEMP);
    write_test_request(request);

    auto host = create_host(&service, &uart_controller);
    while (1) {
        host.run_once();
    }
}

#define UART_TIMEOUT (uint32_t)(1000)

inline Host<ApiRequest, ApiResponse> create_host(ThermoService *service_ptr, StmUartController *uart_controller_ptr) {
    HostBuilder builder;
    builder
        .set_api_request_parser(ipc::ApiRequestParser())
        .set_api_response_serializer(ipc::ApiResponseSerializer())
        .set_raw_data_buffer(&s_buffer)
        .set_raw_data_writer(
            [uart_controller_ptr](const std::vector<std::uint8_t>& data) {
                uart_controller_ptr->write(data);
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

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        throw std::runtime_error("osc cfg failed");
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        throw std::runtime_error("clock cfg failed");
    }
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}