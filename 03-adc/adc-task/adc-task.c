#include "adc-task.h"

#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "stdio.h"

#define PIN_ADC 26
#define CH_ADC 0
#define CH_HS 4


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

