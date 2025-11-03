#include "functions.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "esp_mac.h"

static const char *TAG = "FUNCTIONS";

// Motor pin declartion 
#define DIR_PIN  GPIO_NUM_19
#define PWM_PIN  GPIO_NUM_18
#define FG_PIN   GPIO_NUM_21
// PWM declartion
#define PWM_FREQ 20000
#define PWM_CHANNEL LEDC_CHANNEL_0
#define PWM_TIMER LEDC_TIMER_0
#define PWM_RES LEDC_TIMER_10_BIT

UltrasonicSensor sensors[4] = {
// trig,      echo,       name
    {GPIO_NUM_13,  GPIO_NUM_16, "BS"},
    {GPIO_NUM_14,  GPIO_NUM_17, "BN"},
    {GPIO_NUM_15,  GPIO_NUM_34, "Underbridge"},
    {GPIO_NUM_21,  GPIO_NUM_35, "Car"},
};



void setup_pins() {
    ESP_LOGI(TAG, "Setting up pins...");

// Setup ultrasonic sensor pins
      for (int i = 0; i < 4; i++) {
        gpio_set_direction(sensors[i].trig, GPIO_MODE_OUTPUT);
        gpio_set_direction(sensors[i].echo, GPIO_MODE_INPUT);
    }
// Setup motor control pins
    gpio_reset_pin(DIR_PIN);
    gpio_set_direction(DIR_PIN, GPIO_MODE_OUTPUT);
    gpio_reset_pin(FG_PIN);
    gpio_set_direction(FG_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(FG_PIN);

// PWM setup
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = PWM_RES,
        .timer_num = PWM_TIMER,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = PWM_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = PWM_CHANNEL,
        .timer_sel = PWM_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_conf);

    ESP_LOGI(TAG, "Pins setup complete!");
}



float read_distance_cm(UltrasonicSensor sensor) {
    gpio_set_level(sensor.trig, 0);
    esp_rom_delay_us(2);
    gpio_set_level(sensor.trig, 1);
    esp_rom_delay_us(10);
    gpio_set_level(sensor.trig, 0);

    int64_t timeout_us = 30000; // 30ms max wait (~5m distance)
    int64_t start_time = esp_timer_get_time();

    // Wait for echo high
    while (gpio_get_level(sensor.echo) == 0) {
        if (esp_timer_get_time() - start_time > timeout_us) {
            return -1; // timeout
        }
    }

    int64_t pulse_start = esp_timer_get_time();

    // Wait for echo low
    while (gpio_get_level(sensor.echo) == 1) {
        if (esp_timer_get_time() - pulse_start > timeout_us) {
            return -1; // timeout
        }
    }

    int64_t pulse_end = esp_timer_get_time();
    float distance = (pulse_end - pulse_start) * 0.0343f / 2.0f;

    printf("[%s] Distance: %.2f cm\n", sensor.name, distance);
    return distance;
}





void motor_control(bool openBridge) {
    ESP_LOGI(TAG, "%s", openBridge ? "Opening bridge..." : "Closing bridge...");

    gpio_set_level(DIR_PIN, openBridge ? 1 : 0);

    // Ramp up speed slowly
    for (int duty = 0; duty <= 900; duty += 50) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Hold for a few seconds
    vTaskDelay(pdMS_TO_TICKS(4000));

    // Stop motor
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL);
}



void bridge_sequence() {
    ESP_LOGI(TAG, "Starting bridge sequence...");
//checks the boat sensors for a boat
    float North = read_distance_cm(sensors[0]); 
    float South  = read_distance_cm(sensors[1]); 
    ESP_LOGI(TAG, "sensorname distance: %.2f cm, sensorname distance: %.2f cm", North, South);

// If a boat is detected within 30 cm, initiate bridge sequence
    if ((North > 0 && North < 30.0f) || (South > 0 && South < 30.0f)) {
        ESP_LOGI(TAG, "Boat detected! Stopping traffic and preparing bridge...");
        motor_control(true);
        vTaskDelay(pdMS_TO_TICKS(8000));// Wait for boat to pass
        motor_control(false); // Close bridge
        /*
        check_bridge_clear();
        //need to add LED functionality here
            
        if(check_bridge_clear() == true){  
            motor_control(true); // Open bridge
            vTaskDelay(pdMS_TO_TICKS(8000));// Wait for boat to pass
            motor_control(false); // Close bridge
            ESP_LOGI(TAG, "Bridge sequence complete — traffic may resume.");
        }
        */
    }         
}


/*
bool check_bridge_clear() {
    ESP_LOGI(TAG, "Checking if bridge is clear..."); 
// Continuously monitor car sensors until no car is detected
  while (true) {
        // Read distances from the left and right car sensors
        float leftDistance  = read_distance_cm(sensors[2]);  // Left sensor
        float rightDistance = read_distance_cm(sensors[3]);  // Right sensor
        // Check if cars are present
        bool leftSide  = (leftDistance  > xamount && leftDistance  < yamount); // Adjust xamount and yamount to a proper number just there for a place holder
        bool rightSide = (rightDistance > xamount && rightDistance < yamount);

        if (leftSide && !rightSide) {
            // No cars detected, wait 5 seconds before confirming
            ESP_LOGI(TAG, "No cars detected, waiting 5 seconds to confirm...");
            vTaskDelay(pdMS_TO_TICKS(5000));

            // Double-check
            leftDistance  = read_distance_cm(sensors[2]);
            rightDistance = read_distance_cm(sensors[3]);
            leftSide  = (leftDistance  > xamount && leftDistance  < yamount);
            rightSide = (rightDistance > xamount && rightDistance < yamount);

            if (!leftSide && !rightSide) {
                ESP_LOGI(TAG, "Bridge is clear!");
                return true;  // Safe to open bridge
            } else {
                ESP_LOGI(TAG, "Car detected during confirmation, continuing to monitor...");
            }
        } else {
            ESP_LOGI(TAG, "Car detected! Left: %.2f cm, Right: %.2f cm", leftDistance, rightDistance);
        }

        // Wait a short period before checking again
        vTaskDelay(pdMS_TO_TICKS(500));
    }
        
   
}
*/

