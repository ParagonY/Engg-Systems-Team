
#define DIR_PIN   GPIO_NUM_5
#define PWM_PIN   GPIO_NUM_12

#define PWM_FREQ  20000
#define PWM_RES   LEDC_TIMER_8_BIT
#define PWM_TIMER LEDC_TIMER_0
#define PWM_CH    LEDC_CHANNEL_0

static int current_speed = 0;
static int current_dir = 1;

void motor_pinset() {
    gpio_set_direction(DIR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DIR_PIN, 1);

    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = PWM_RES,
        .timer_num = PWM_TIMER,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = PWM_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = PWM_CH,
        .timer_sel = PWM_TIMER,
        .duty = 255,  // motor off (FIT0441 is inverted)
        .hpoint = 0
    };
    ledc_channel_config(&channel);

    ESP_LOGI(TAG, "Motor initialized.");
}

void motor_set(int speed, int dir) {
    current_speed = speed;
    current_dir = dir;
    gpio_set_level(DIR_PIN, dir);
    int duty = 255 - speed; // FIT0441 inverted logic
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CH, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CH);
}

void motorChangeState(string a){
    if(a == "LIFT"){
        motor_set(0,1);
        vTaskDelay(pdMS_TO_TICKS(3000));
        motor_set(255,0);
        LEDChangeState("GREENB");
        }
    
    if(a == "DOWN"){
        motor_set(0,0);
        vTaskDelay(pdMS_TO_TICKS(3000));
        motor_set(255,0);
        LEDChangeState("GREENC");
    }
    if(a == "STOP"){
        motor_set(255,0);
        LEDChangeState("REDA");
    }
}

void motor_stop() {
    motor_set(0, current_dir);
    ESP_LOGI(TAG, "Motor stopped.");
}

/*
if (strcmp(received_command_ptr, "open") == 0) {
    ESP_LOGI(TAG, "Bridge opening...");
    motor_set(180, 1); // forward, controlled speed
}
else if (strcmp(received_command_ptr, "close") == 0) {
    ESP_LOGI(TAG, "Bridge closing...");
    motor_set(180, 0); // reverse
}
else if (strcmp(received_command_ptr, "stop") == 0) {
    ESP_LOGI(TAG, "Bridge stopping...");
    motor_stop();
}

not sure where this needs to go but this is the if statement for the motor commands works all through thr funtion 
motor_set were able to set the speed and direction of the motor

i guess motor_pinset is unecessary since theres already the other setup pin function but ill leave it here for now
*/