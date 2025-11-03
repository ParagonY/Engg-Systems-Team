
// 
// Working sensor code as of 3/11/2025
//

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"
    #include "esp_timer.h"
    #include "esp_log.h"
}

#define TRIG_PIN GPIO_NUM_13
#define ECHO_PIN GPIO_NUM_16

extern "C" void app_main(void)
{
    gpio_reset_pin(TRIG_PIN);
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ECHO_PIN);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);

    printf("Ultrasonic sensor test started!\n");

    while (true) {
        int64_t start = 0;
        int64_t end = 0;
        int64_t duration = 0;
        float distance_cm = 0.0f;

        // Send trigger pulse
        gpio_set_level(TRIG_PIN, 0);
        esp_rom_delay_us(2);
        gpio_set_level(TRIG_PIN, 1);
        esp_rom_delay_us(10);
        gpio_set_level(TRIG_PIN, 0);

        // Wait for echo start (timeout)
        int64_t wait_start = esp_timer_get_time();
        while (gpio_get_level(ECHO_PIN) == 0) {
            if (esp_timer_get_time() - wait_start > 30000) { // 30ms timeout
                printf("Timeout waiting for echo HIGH\n");
                goto delay_next;
            }
        }

        start = esp_timer_get_time();

        // Wait for echo end (timeout)
        while (gpio_get_level(ECHO_PIN) == 1) {
            if (esp_timer_get_time() - start > 30000) {
                printf("Timeout waiting for echo LOW\n");
                goto delay_next;
            }
        }

        end = esp_timer_get_time();

        // Calculate distance
        duration = end - start;
        distance_cm = (duration * 0.0343f) / 2.0f;

        printf("Distance: %.2f cm\n", distance_cm);

    delay_next:
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}