#include "pico/stdlib.h"
#include "hardware/gpio.h"

const uint LED_PIN = 25;

int main()
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while(1)
    {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        sleep_ms(500);
    }
}