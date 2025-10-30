#pragma once

struct UltrasonicSensor {       // Struct to hold ultrasonic sensor pin configuration allows us in main to define multiple sensors easily 
    gpio_num_t trig;            
    gpio_num_t echo;          
    const char* name;
};

//sensor array
extern UltrasonicSensor sensors[4]; 

// Function declarations
void setup_pins();
float read_distance_cm(UltrasonicSensor sensor); 
void motor_control(bool openBridge);
void bridge_sequence();
bool check_bridge_clear();