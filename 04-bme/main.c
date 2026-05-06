#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task.h"
#include "bme280-driver.h"
#include "led-task/led-task.h"
#include "adc-task/adc-task.h"

#define DEVICE_NAME "MASTERS_MC"
#define DEVICE_VRSN "PRIME_VERSION"


void help_callback(const char* args);
void version_callback(const char* args);
void mem_callback(const char* args);
void wmem_callback(const char* args);
void led_callback(const char* args);
void led_period_callback(const char* args);
void get_adc_callback(const char* args);
void get_temp_callback(const char* args);
void tm_start_callback(const char* args);
void tm_stop_callback(const char* args);
void read_reg_callback(const char* args);
void write_reg_callback(const char* args);
void temp_raw_callback(const char* args);
void press_raw_callback(const char* args);
void hum_raw_callback(const char* args);
void temp_callback(const char* args);


api_t device_api[] =
{
	{"help", help_callback, "prints commands description"},
    {"version", version_callback, "get device name and firmware version"},
    {"mem", mem_callback, "read from memory from hex addres"},
    {"wmem", wmem_callback, "write hex number in memory from hex addres"},
    {"led", led_callback, "changes led state depended of an argument (0 - off, 1 - on, 2 - blink)"},
    {"led_period", led_period_callback, "sets led blink frequency depended of an argument (in us)"},
    {"get_adc", get_adc_callback, "mesuares voltage on pin 26 in range 0-3.3V"},
    {"get_temp", get_temp_callback, "mesuares local MC temperature in Celcious"},
    {"tm_start", tm_start_callback, "starts sending telemetry [voltage_V on GPIO26; temp_C on chip] in Serial port"},
    {"tm_stop", tm_stop_callback, "stops sending telemetry"},
    {"read_reg", read_reg_callback, "read <arg2> registers from address <arg1> (both args can't be greater than 0xFF)"},
    {"write_reg", write_reg_callback, "write <arg2> to address <arg1>  (both args can't be greater than 0xFF)"},
    {"temp_raw", temp_raw_callback, "returns temperature raw value from bme280"},
    {"press_raw", press_raw_callback, "returns pressure raw value from bme280"},
    {"hum_raw", hum_raw_callback, "returns humidity raw value from bme280"},
    {"temp", temp_callback, "returns temperature value in DegC from bme280"},
	{NULL, NULL, NULL},
};


void help_callback(const char* args)
{
    for (int i = 0; device_api[i].command_name; i++)
    {
        printf("\nCommand '%s' \t: %s", device_api[i].command_name, device_api[i].command_help);
    }
    printf("\n");
    return;
}

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void mem_callback(const char* args)
{
    if (args == NULL || args[0] == '\0') {
        printf("Error: invalid address or too few arguments\n");
        return;
    }
    
    uint32_t addr;
    sscanf(args, "%x", &addr);

    volatile uint32_t* ptr = (volatile uint32_t*)addr;
    printf("Value at 0x%X = %x\n", addr, *ptr);
    return;
}

void wmem_callback(const char* args)
{
    uint32_t addr, value;
    sscanf(args, "%x %x", &addr, &value);
    if (args == NULL || args[0] == '\0' || addr == 0 || addr % 4 != 0) {
        printf("Error: invalid address or too few arguments\n");
        return;
    }
    *(volatile uint32_t*)addr = value;
}

void led_callback(const char* args)
{
    uint32_t num;
    led_state_t state;
    sscanf(args, "%u", &num);
    if (2 < num)
    {
        printf("Error: invalid argument %u\n", num);
        return;
    }
    switch(num){
        case 0:
            state = LED_STATE_OFF;
            break;
        case 1:
            state = LED_STATE_ON;
            break;
        case 2:
            state = LED_STATE_BLINK;
            break;
    }
    led_task_state_set(state);
}

void led_period_callback(const char* args)
{
    uint32_t period;
    sscanf(args, "%u", &period);
    if (!period)
    {
        printf("Error: invalid argument");
        return;
    }
    led_task_period_set(period);
    return;
}

void get_adc_callback(const char* args)
{
    float voltage_V = adc_task_measure();
    printf("%f\n", voltage_V);
}

void get_temp_callback(const char* args)
{
    float temp_C = adc_task_tempm();
    printf("%f\n", temp_C);
}

void tm_start_callback(const char* args)
{
    adc_task_set_state(ADC_TASK_STATE_RUN);
}

void tm_stop_callback(const char* args)
{
    adc_task_set_state(ADC_TASK_STATE_IDLE);
}

void read_reg_callback(const char* args)
{
    uint32_t addr, N;
    sscanf(args, "%x %x", &addr, &N);
    if (addr > 0xFF || N > 0xFF || addr + N > 0x100)
    {
        printf("Error: invalid arguments: %x, %x\n", addr, N);
        return;
    }   
    uint8_t buffer[256] = {0};
    bme280_read_regs((uint8_t)addr, buffer, (uint8_t)N); 
    for (int i = 0; i < N; i++)
    {
        printf("bme280 register [0x%X] = 0x%X\n", addr + i, buffer[i]);
    }
}

void write_reg_callback(const char* args)
{
    uint32_t addr, value;
    sscanf(args, "%x %x", &addr, &value);
    if (addr > 0xFF || value > 0xFF)
    {
        printf("Error: invalid arguments: %x, %x\n", addr, value);
        return;
    }   
    bme280_write_reg((uint8_t)addr, (uint8_t)value);
}

void temp_raw_callback(const char* args)
{
    printf("Temperature (raw) = %u \n", bme280_read_temp_raw());
}

void press_raw_callback(const char* args)
{
    printf("Pressure (raw) = %u \n", bme280_read_press_raw());
}

void hum_raw_callback(const char* args)
{
    printf("Humidity (raw) = %u \n", bme280_read_hum_raw());
}

void temp_callback(const char* args)
{
    float temp_DegC = bme280_read_temp();
    printf("Temperature = %f DegC \n", temp_DegC);
}

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
	i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}

void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
	i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
}

int main()
{
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_init();
    adc_task_init();
    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    bme280_init(rp2040_i2c_read, rp2040_i2c_write);
    while(1)    
    {
        char* line = stdio_task_handle();
        protocol_task_handle(line);
        led_handler();
        adc_task_handle();
    }
}