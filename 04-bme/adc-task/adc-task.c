#include "adc-task.h"

#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "stdio.h"

#define PIN_ADC 26
#define CH_ADC 0
#define CH_HS 4

adc_task_state_t adc_state = ADC_TASK_STATE_IDLE;   
uint64_t ts = 0;
uint64_t ADC_TASK_MEAS_PERIOD_US = 100000;

void adc_task_init()
{
    adc_init();
    adc_gpio_init(PIN_ADC);
    adc_set_temp_sensor_enabled(true);
    return;
}

float adc_task_measure()
{
    adc_select_input(CH_ADC);
    uint16_t voltage_counts = adc_read();
    return 3.3 * (float)voltage_counts / 4096;
}

float adc_task_tempm()
{
    adc_select_input(CH_HS);
    uint16_t temp_counts = adc_read();
    return 27.0f - (3.3f * (float)temp_counts / 4096.0f - 0.706f) / 0.001721f;
}

void adc_task_set_state(adc_task_state_t state)
{
    adc_state = state;
}

void adc_task_handle()
{
    switch(adc_state)
    {
    case ADC_TASK_STATE_IDLE:
        break;
    case ADC_TASK_STATE_RUN:
        if (time_us_64() > ts)
        {
            ts = time_us_64() + ADC_TASK_MEAS_PERIOD_US;
            float voltage_V = adc_task_measure();
            float temp_C = adc_task_tempm();
            printf("%f %f\n", voltage_V, temp_C);
        }
        break;
    default:
        break;
    }
}