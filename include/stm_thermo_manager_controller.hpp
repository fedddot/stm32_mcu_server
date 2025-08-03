#ifndef STM_THERMO_MANAGER_CONTROLLER_HPP
#define STM_THERMO_MANAGER_CONTROLLER_HPP

#include "stm_timer_controller.hpp"
#include "thermostat_controller.hpp"

namespace stm32 {
    class StmThermoManagerController : public service::ThermostatController {
    public:
        StmThermoManagerController(double scale_factor = 1.0)
            : m_scale_factor(scale_factor)
        {
            // Enable GPIOC clock for relay
            RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
            GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
            GPIOC->CRH |= (GPIO_CRH_MODE13_1 | GPIO_CRH_MODE13_0);
            GPIOC->CRH &= ~GPIO_CRH_CNF13;

            // Enable GPIOA and ADC1 clocks
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

            // Configure PA0 (ADC channel 0) as analog input
            GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
            GPIOA->CRL |= GPIO_CRL_CNF0_1; // Analog mode

            // ADC1 configuration
            ADC1->CR2 = 0;
            ADC1->CR1 = 0;
            ADC1->SMPR2 |= ADC_SMPR2_SMP0; // Sample time for channel 0 (max cycles)
            ADC1->CR2 |= ADC_CR2_ADON; // Enable ADC
            // ADC calibration
            ADC1->CR2 |= ADC_CR2_RSTCAL;
            while (ADC1->CR2 & ADC_CR2_RSTCAL) {}
            ADC1->CR2 |= ADC_CR2_CAL;
            while (ADC1->CR2 & ADC_CR2_CAL) {}
        }
        double read_temperature() const override {
            // Start conversion on channel 0 (PA0)
            ADC1->SQR3 = 0; // Channel 0
            ADC1->CR2 |= ADC_CR2_ADON; // Start conversion
            while (!(ADC1->SR & ADC_SR_EOC)) {}
            uint16_t adc_value = ADC1->DR;
            // Scale the ADC value
            return static_cast<double>(adc_value) * m_scale_factor;
        }
        void set_relay_state(const bool state) override {
            if (state) {
                GPIOC->BSRR = GPIO_BSRR_BS13; // Set PC13 high
            } else {
                GPIOC->BSRR = GPIO_BSRR_BR13; // Set PC13 low
            }
        }
        TaskGuard *schedule_task(const Task& task, const std::size_t period_ms) override {
            m_timer_controller.schedule_task(task, period_ms);
            return new TimerTaskGuard(&m_timer_controller);
        }
    private:
        StmTimerController m_timer_controller;
        double m_scale_factor;

        class TimerTaskGuard: public TaskGuard {
        public:
            TimerTaskGuard(StmTimerController *timer_controller_ptr) : m_timer_controller_ptr(timer_controller_ptr) {}
            void unschedule() override {
                m_timer_controller_ptr->unschedule_task();
            }
        private:
            StmTimerController *m_timer_controller_ptr;
        };
    };
}

#endif // STM_THERMO_MANAGER_CONTROLLER_HPP
