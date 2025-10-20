extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"
    #include "driver/ledc.h"
    #include "esp_timer.h"
    #include "esp_log.h"
    #include "esp_rom_sys.h"   
}

#include <stdio.h>

#define TRIG_PIN GPIO_NUM_4   
#define ECHO_PIN GPIO_NUM_2  
#define DIR_PIN  GPIO_NUM_5
#define PWM_PIN  GPIO_NUM_12