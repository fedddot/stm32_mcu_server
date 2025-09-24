#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "api_message_reader.hpp"
#include "api_message_writer.hpp"
#include "host.hpp"
#include "package_reader.hpp"
#include "package_header_parser.hpp"
#include "package_header_serializer.hpp"
#include "package_writer.hpp"
#include "ring_buffer_input_stream.hpp"
#include "stepper_request.hpp"
#include "stepper_response.hpp"
#include "stepper_service.hpp"
#include "stepper_types.hpp"
#include "stm_uart_controller.hpp"
#include "pb_stepper_request_parser.hpp"
#include "pb_stepper_response_serializer.hpp"
#include "stm32_stepper_provider.hpp"

#include "stm32f103xb.h"

#ifndef DATA_BUFFER_SIZE_CFG
#  error "DATA_BUFFER_SIZE_CFG is not defined"
#endif
#ifndef PREAMBLE_CFG
#  error "PREAMBLE_CFG is not defined"
#endif
#ifndef PREAMBLE_SIZE_CFG
#  error "PREAMBLE_SIZE_CFG is not defined"
#endif
#ifndef ENCODED_PAYLOAD_SIZE_LENGTH_CFG
#  error "ENCODED_PAYLOAD_SIZE_LENGTH is not defined"
#endif

using namespace host;
using namespace service;
using namespace ipc;

using StmPackageReader = PackageReader<PREAMBLE_SIZE_CFG, ENCODED_PAYLOAD_SIZE_LENGTH_CFG>;
using StmPackageWriter = PackageWriter<PREAMBLE_SIZE_CFG, ENCODED_PAYLOAD_SIZE_LENGTH_CFG>;

RingBufferInputStream<std::uint8_t, DATA_BUFFER_SIZE_CFG> s_buffer;

static void init_clock();

int main(void) {
    init_clock();
    StmUartController uart_controller(
        [buff_ptr = &s_buffer](const std::uint8_t byte) {
            buff_ptr->enqueue(byte);
        }
    );
    StmPackageReader::Preamble preamble;
    if (nullptr == std::memcpy(preamble.data(), PREAMBLE_CFG, PREAMBLE_SIZE_CFG)) {
        throw std::runtime_error("failed to copy preamble");
    }
    StmPackageReader package_reader(
        &s_buffer,
        preamble,
        PackageHeaderParser<PREAMBLE_SIZE_CFG, ENCODED_PAYLOAD_SIZE_LENGTH_CFG>()        
    );
    StmPackageWriter package_writer(
        [uart_controller_ptr = &uart_controller](const std::vector<std::uint8_t>& data) {
            uart_controller_ptr->write(data);
        },
        static_cast<StmPackageWriter::Preamble>(preamble),
        PackageHeaderSerializer<PREAMBLE_SIZE_CFG, ENCODED_PAYLOAD_SIZE_LENGTH_CFG>()
    );
    ApiMessageReader<StepperRequest> request_reader(
        &package_reader,
        PbStepperRequestParser()
    );
    ApiMessageWriter<StepperResponse> response_writer(
        &package_writer,
        PbStepperResponseSerializer()
    );
    StepperService service(nullptr);
    Host<StepperRequest, StepperResponse> host(
        &request_reader,
        &response_writer,
        &service,
        [](const std::exception& e) {
            return StepperResponse(
                StepperResponse::Result::FAILURE,
                State::DISABLED,
                std::string(e.what())
            );
        }
    );
    while (true) {
        host.run_once();
    }
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