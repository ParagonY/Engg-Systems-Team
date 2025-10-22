
#include <stdio.h>

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"
    #include "driver/ledc.h"
    #include "esp_timer.h"
    #include "esp_rom_sys.h"
}

// --- Pin definitions ---
#define TRIG_PIN GPIO_NUM_2   
#define ECHO_PIN GPIO_NUM_4   
#define DIR_PIN  GPIO_NUM_5   // Motor direction pin foward-reverse 1 or HIGH = forward 0 or LOW = reverse
#define PWM_PIN  GPIO_NUM_12  // Motor PWM control pin the speed of the motor range 0-255 0 = stop 255 = full speed

float get_distance_cm() {
    // Send a short trigger pulse
    gpio_set_level(TRIG_PIN, 0);
    esp_rom_delay_us(2);
    gpio_set_level(TRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIG_PIN, 0);

    // Wait for echo to start (timeout after 30ms)
    int timeout = 30000;
    while (gpio_get_level(ECHO_PIN) == 0 && timeout-- > 0) {
        esp_rom_delay_us(1);
    }
    if (timeout <= 0) return -1;  // No echo start detected

    int64_t start_time = esp_timer_get_time();

    // Wait for echo to end
    timeout = 30000;
    while (gpio_get_level(ECHO_PIN) == 1 && timeout-- > 0) {
        esp_rom_delay_us(1);
    }
    if (timeout <= 0) return -1;  // No echo end detected

    int64_t end_time = esp_timer_get_time();

    // Calculate distance in cm
    float duration = (float)(end_time - start_time);
    float distance_cm = (duration * 0.0343f) / 2.0f;

    return distance_cm;
}

extern "C" void app_main(void) // like a main() function in java 
{
    // --- Setup pins for ultrasonic sensor ---
    gpio_reset_pin(TRIG_PIN);
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ECHO_PIN);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);

    // --- Setup motor control pins ---
    gpio_reset_pin(DIR_PIN);
    gpio_set_direction(DIR_PIN, GPIO_MODE_OUTPUT);

    // --- Setup PWM (LEDC) for motor speed control ---
    ledc_timer_config_t pwm_timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = 20000,        // 20 kHz PWM frequency
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ledc_timer_config(&pwm_timer);

    ledc_channel_config_t pwm_channel = {
        .gpio_num   = PWM_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 1,                // Start with motor off
        .hpoint     = 0
    };
    ledc_channel_config(&pwm_channel);

    printf("\nUltrasonic sensor and motor test started!\n");

    while (true) {
        // Measure distance
        float distance = get_distance_cm();

        if (distance < 0) {
            printf("No object detected (timeout)\n");
        } else {
            printf("Distance: %.2f cm\n", distance);
        }

        // If something is close (<20cm), run motor for 3s
        if (distance > 0 && distance < 20.0f) {
            printf("Object detected nearby! Turning motor ON...\n");

            gpio_set_level(DIR_PIN, 1);  // Set direction
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 255); // Full speed
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

            vTaskDelay(pdMS_TO_TICKS(3000)); // Run for 3 seconds

            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);   // Stop motor
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

            printf("Motor stopped.\n");
        }

        // Wait before next measurement
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


// change varibale names 
// motor run funtion 
//
