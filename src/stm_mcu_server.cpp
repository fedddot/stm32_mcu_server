#include <optional>
#include <stdexcept>

#include "ipc_data.hpp"
#include "ipc_data_writer.hpp"
#include "ipc_instance.hpp"
#include "relay_controller.hpp"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "main.h"

#include "temperature_sensor_controller.hpp"
#include "thermostat_host_builder.hpp"
#include "timer_scheduler.hpp"
#include "proto_thermostat_api_request_parser.hpp"
#include "proto_thermostat_api_response_serializer.hpp"

using namespace host;
using namespace vendor;
using namespace ipc;
using namespace manager;

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);

extern "C" {
    int main(void);
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

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    ThermostatHostBuilder builder;
    builder
        .set_api_request_parser(ApiRequestParser())
        .set_api_response_serializer(ApiResponseSerializer())
        .set_raw_data_reader(ThermostatHostBuilder::RawDataReaderInstance(new RawDataReader()))
        .set_raw_data_writer(ThermostatHostBuilder::RawDataWriterInstance(new RawDataWriter()))
        .set_relay_controller(ThermostatHostBuilder::RelayControllerInstance(new StmRelayController()))
        .set_scheduler(ThermostatHostBuilder::SchedulerInstance(new StmTimerScheduler()))
        .set_temp_sensor(ThermostatHostBuilder::SensorInstance(new StmTempSensor()));

    
    auto host = builder.build();
    
    while (1) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        host.run_once();
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
    |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
* @brief GPIO Initialization Function
* @param None
* @retval None
*/
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    
    /* USER CODE END MX_GPIO_Init_1 */
    
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);
    
    /*Configure GPIO pin : LED_PIN_Pin */
    GPIO_InitStruct.Pin = LED_PIN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_PIN_GPIO_Port, &GPIO_InitStruct);
    
    /* USER CODE BEGIN MX_GPIO_Init_2 */
    
    /* USER CODE END MX_GPIO_Init_2 */
}

void Error_Handler(void) {
    throw std::runtime_error("ERROR");
}