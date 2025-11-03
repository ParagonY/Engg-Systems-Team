
// // --- ESP-IDF POSIX TCP Client in ACCESS POINT (AP) Mode with 3 FreeRTOS Tasks ---

// #include <iostream>
// #include <cstring>
// #include <cstdio>
// #include <cstdlib>
// #include <sys/socket.h>  // POSIX sockets
// #include <netdb.h>       
// #include <unistd.h>      // For close()
// #include <string>        // Used for easier string handling

// // ESP-IDF specific headers
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"      
// #include "freertos/event_groups.h" 
// #include "esp_log.h"     
// #include "esp_event.h"   
// #include "esp_wifi.h"    
// #include "nvs_flash.h"   
// #include "lwip/ip4_addr.h" 

// static const char *TAG = "ESP-Client";

// #define Port 5050
// #define IPAdress "192.168.4.2" 
// #define BufferSize 1024

// #define SSIDName         "ESP32_UI_NETWORK"    
// #define ConnectionPassword     "password123" 

// static EventGroupHandle_t wifiGroupHandle;
// const int APFirstBit = BIT0; 

// static QueueHandle_t recivedMessageList = NULL;
// static int g_socket_fd = -1; 


// // static void tcp_listener_task(void *pvParameters); 
// // static void command_processor_task(void *pvParameters); 
// // static void hardware_monitor_task(void *pvParameters); 


// static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
//     if (event_base == WIFI_EVENT) {
//         if (event_id == WIFI_EVENT_AP_START) {
//             ESP_LOGI(TAG, "Access Point started successfully.");
//             xEventGroupSetBits(wifiGroupHandle, APFirstBit);
//         } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
//             ESP_LOGI(TAG, "PC connected to Access Point.");
//         } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
//             ESP_LOGW(TAG, "PC disconnected from Access Point. Connection will fail.");
//         }
//     }
// }

// // --- ACCESS POINT Initialization Logic ---
// void wifi_init_ap(void) {
//     wifiGroupHandle = xEventGroupCreate();
    
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_ap();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&event_handler,NULL,NULL));

//     wifi_config_t wifi_config = {}; 
//     strcpy((char*)wifi_config.ap.ssid, SSIDName);
//     strcpy((char*)wifi_config.ap.password, ConnectionPassword);
//     wifi_config.ap.ssid_len = strlen(SSIDName);
//     wifi_config.ap.max_connection = 1;
//     wifi_config.ap.authmode = WIFI_AUTH_WPA2_WPA3_PSK; 
    
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());

//     ESP_LOGI(TAG, "Access Point creation initiated. SSID: %s, IP: 192.168.4.1", SSIDName);
//     xEventGroupWaitBits(wifiGroupHandle, APFirstBit, pdFALSE, pdTRUE, portMAX_DELAY);
//     ESP_LOGI(TAG, "Access Point ready. Please connect your PC to '%s'.", SSIDName);
// }


// // --- TASK 3: Hardware Monitor Task (Independent Local Control/HW Checks) ---
// static void hardware_monitor_task(void *pvParameters) {
//     // NOTE: This task represents your continuous local hardware monitoring and safety checks.
    
//     // Placeholder for hardware initialization (e.g., setup_pins();)
//     ESP_LOGI("HW_MONITOR", "Hardware initialized. Starting local checks...");
    
//     while (1) {
//         ESP_LOGI("HW_MONITOR", "Hello, C++! Running theoretical hardware code.");
//         // Placeholder for running continuous local logic (e.g., bridge_sequence();)
//         ESP_LOGD("HW_MONITOR", "Running local safety checks and sensor readings.");
        
//         // This task runs completely independently of network traffic.
//         //vTaskDelay(pdMS_TO_TICKS(1500)); 
//     }
// }


// // --- TASK 2: Command Processor Task (Consumer, Dispatcher, and Sender) ---
// static void command_processor_task(void *pvParameters) {
//     char* received_command_ptr;
    
//     while (1) {
//         // Try to receive a command from the queue (wait for a short period)
//         if (xQueueReceive(recivedMessageList, &received_command_ptr, pdMS_TO_TICKS(100)) == pdPASS) {
            
//             ESP_LOGI("PROCESSOR", "==> Processing Command: %s", received_command_ptr);
            
//             // --- 1. PROCESS RECEIVED COMMAND ---
//             const char* reply_message = "ACK: UNKNOWN_COMMAND\n";

//             if (strcmp(received_command_ptr, "OPEN_BRIDGE") == 0) {
//                 ESP_LOGW("PROCESSOR", "Action: Initiating bridge opening sequence!");
//                 // You would call your hardware control function here (e.g., motor_control(true))
//                 reply_message = "ACK: BRIDGE_OPENING\n";
//             } else if (strcmp(received_command_ptr, "STATUS_REQUEST") == 0) {
//                 ESP_LOGI("PROCESSOR", "Action: Gathering status data.");
//                 // You would get real status data from your HW and format it here
//                 reply_message = "STATUS: BRIDGE_CLOSED\n"; 
//             }
            
//             // --- 2. SEND A REPLY MESSAGE (if socket is active) ---
//             if (g_socket_fd != -1) {
//                 send(g_socket_fd, reply_message, strlen(reply_message), 0);
//                 ESP_LOGI("PROCESSOR", "Sent reply: %s", reply_message);
//             } else {
//                 ESP_LOGW("PROCESSOR", "Cannot send reply, socket is currently closed/reconnecting.");
//             }
            
//             // 3. Free the memory allocated by the Listener Task
//             free(received_command_ptr);
//             // --------------------------------------------------------

//         } else {
//             // No command received this cycle. Wait for the next one.
//             vTaskDelay(pdMS_TO_TICKS(50)); 
//         }
//     }
// }


// // --- TASK 1: TCP Listener Task (Network Handler and Producer) ---
// static void tcp_listener_task(void *pvParameters) {
//     // This outer loop handles connection retries if the persistent connection breaks.
//     while (1) { 
//         struct addrinfo hints = {}; 
//         hints.ai_family = AF_INET;
//         hints.ai_socktype = SOCK_STREAM;
        
//         struct addrinfo *res = NULL;
//         int s = -1;
        
//         char port_str[6];
//         sprintf(port_str, "%d", Port);
        
//         char recvBuffer[BufferSize];
//         int bytesReceived;

//         ESP_LOGI(TAG, "Attempting connection to PC Server: %s:%d", IPAdress, Port);
        
//         // Connection setup logic (omitted for brevity)
//         int err = getaddrinfo(IPAdress, port_str, &hints, &res);
//         if (err != 0 || res == NULL) { goto retry; }
//         s = socket(res->ai_family, res->ai_socktype, 0);
//         if (s < 0) { goto cleanup; }
//         if (connect(s, res->ai_addr, res->ai_addrlen) != 0) { goto cleanup; }
        
//         ESP_LOGI(TAG, "Successfully connected to PC Server and keeping connection open!");
        
//         // SET GLOBAL SOCKET (Allows the Processor Task to send replies)
//         g_socket_fd = s;
        
//         // 4. PERSISTENT COMMUNICATION LOOP (Listening for Commands)
//         while (1) {
//             // Set a timeout so we don't block forever, allowing loop to check for errors/status
//             struct timeval timeout = { .tv_sec = 2, .tv_usec = 0 };
//             setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

//             bytesReceived = recv(s, recvBuffer, BufferSize - 1, 0);
            
//             if (bytesReceived > 0) {
//                 recvBuffer[bytesReceived] = '\0'; 
//                 ESP_LOGI(TAG, "Received %d bytes: %s", bytesReceived, recvBuffer);

//                 // --- ADD RECEIVED MESSAGE TO QUEUE (The Producer) ---
//                 char* command_copy = (char*)malloc(bytesReceived + 1);
//                 if (command_copy) {
//                     strcpy(command_copy, recvBuffer);

//                     // Send the pointer to the command copy to the queue
//                     if (xQueueSend(recivedMessageList, (void*)&command_copy, pdMS_TO_TICKS(10)) != pdPASS) {
//                         ESP_LOGE(TAG, "Queue full! Dropping command and freeing memory.");
//                         free(command_copy); 
//                     }
//                 }
//                 // --------------------------------------------------------

//             } else if (bytesReceived == 0) {
//                 ESP_LOGW(TAG, "Connection closed gracefully by PC Server.");
//                 break; // Exit communication loop to trigger cleanup/retry
//             } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
//                 // Hard error occurred
//                 ESP_LOGE(TAG, "recv failed: errno %d. Connection lost.", errno);
//                 break; // Exit communication loop to trigger cleanup/retry
//             } 

//             // Allow other tasks to run
//             vTaskDelay(pdMS_TO_TICKS(100)); 
//         }

//     cleanup:
//         // Connection broken or error occurred. Clean up socket and reset global flag.
//         if (s != -1) { 
//             close(s); 
//             g_socket_fd = -1; // Reset global socket handle
//             s = -1; 
//             ESP_LOGI(TAG, "Socket closed and global handle reset.");
//         }
//         if (res != NULL) {
//             freeaddrinfo(res);
//         }

//     retry:
//         ESP_LOGI(TAG, "Waiting 10s before attempting connection retry...");
//         vTaskDelay(pdMS_TO_TICKS(10000));
//     }

//     vTaskDelete(NULL); 
// }


// // --- Main ESP-IDF Entry Point (The Initializer/Starter Thread) ---

// extern "C" void app_main(void) {
//     // 0. Initialize NVS and Wi-Fi
//     esp_err_t ret = nvs_flash_init();
//     // (Error checking and Wi-Fi init omitted for brevity but present)
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     ESP_LOGI(TAG, "ESP32 AP mode starting up...");
//     wifi_init_ap();
    
//     // 1. CREATE THE QUEUE (The central buffer for network commands)
//     recivedMessageList = xQueueCreate(5, sizeof(char*));
//     if (recivedMessageList == NULL) {
//         ESP_LOGE(TAG, "Failed to create command queue! System halt.");
//         return; 
//     }

//     // 2. Start the three concurrent tasks
    
//     // Task 1: TCP Listener (Connects, Listens, Produces commands to queue)
//     xTaskCreate(tcp_listener_task, "listener", 4096, NULL, 5, NULL);
    
//     // Task 2: Command Processor (Consumes from queue, processes, sends replies)
//     xTaskCreate(command_processor_task, "processor", 4096, NULL, 5, NULL);
    
//     // Task 3: Hardware Monitor (Independent loop for local logic and safety checks)
//     xTaskCreate(hardware_monitor_task, "hw_monitor", 4096, NULL, 4, NULL);
    
//     // app_main returns, and the scheduler takes over running the three tasks.
// }











// ESP-IDF (FreeRTOS) code for reading an HC-SR04 Ultrasonic Sensor
// Trig Pin: GPIO 21 (Output)
// Echo Pin: GPIO 35 (Input)

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h" // For microsecond-accurate timing

static const char *TAG = "ULTRASONIC";

// --- PIN DEFINITIONS ---
#define ULTRASONIC_TRIG_PIN GPIO_NUM_21
#define ULTRASONIC_ECHO_PIN GPIO_NUM_35

// --- SPEED OF SOUND (cm/µs) ---
// Speed of sound in air is approx 343m/s or 0.0343 cm/µs
#define SOUND_SPEED_CM_PER_US 0.0343f

/**
 * @brief Measures the distance using the HC-SR04 sensor.
 * * @return float Distance in centimeters, or -1.0 if timeout/error occurs.
 */
float measure_distance() {
    // 1. Ensure the trigger is low initially
    gpio_set_level(ULTRASONIC_TRIG_PIN, 0);
    // Short delay to settle
    esp_rom_delay_us(2); 

    // 2. Generate 10µs pulse on the TRIG pin
    gpio_set_level(ULTRASONIC_TRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(ULTRASONIC_TRIG_PIN, 0);

    int64_t start_time = 0;
    int64_t end_time = 0;
    
    // --- Pulse Measurement ---
    
    // 3. Wait for the ECHO pin to go high (start of the pulse)
    // Timeout set to 20ms (20000 µs) for safety
    int64_t pulse_start_timeout = esp_timer_get_time() + 20000; 
    while (gpio_get_level(ULTRASONIC_ECHO_PIN) == 0) {
        if (esp_timer_get_time() > pulse_start_timeout) {
            ESP_LOGW(TAG, "Echo pulse start timeout.");
            return -1.0f; 
        }
    }
    start_time = esp_timer_get_time();

    // 4. Wait for the ECHO pin to go low (end of the pulse)
    // Max measurement time for 4m is about 25,000 µs
    int64_t pulse_end_timeout = start_time + 30000;
    while (gpio_get_level(ULTRASONIC_ECHO_PIN) == 1) {
        if (esp_timer_get_time() > pulse_end_timeout) {
            ESP_LOGW(TAG, "Echo pulse end timeout (Max range exceeded?).");
            return -1.0f;
        }
    }
    end_time = esp_timer_get_time();
    
    // 5. Calculate Duration
    int64_t duration_us = end_time - start_time;

    // 6. Calculate Distance
    // Distance = (Duration * Speed of Sound) / 2 (since it's round trip)
    float distance_cm = (float)duration_us * SOUND_SPEED_CM_PER_US / 2.0f;

    return distance_cm;
}

/**
 * @brief FreeRTOS task to continuously monitor the ultrasonic sensor.
 */
void ultrasonic_task(void *pvParameters) {
    while (1) {
        float distance = measure_distance();
        
        if (distance > 0.0f) {
            // Print the distance, limiting to two decimal places
            ESP_LOGI(TAG, "Distance: %.2f cm", distance);
        }
        
        // Wait for 500ms before the next measurement
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Initializes GPIOs and starts the monitoring task.
 */
extern "C" void app_main(void) {
    // 1. Configure Trigger Pin (Output)
    gpio_set_direction(ULTRASONIC_TRIG_PIN, GPIO_MODE_OUTPUT);

    // 2. Configure Echo Pin (Input with pull-down just in case)
    gpio_set_direction(ULTRASONIC_ECHO_PIN, GPIO_MODE_INPUT);
    gpio_pullup_dis(ULTRASONIC_ECHO_PIN); // Disable pull-up
    gpio_pulldown_dis(ULTRASONIC_ECHO_PIN); // For GPIO35, this is not needed as it's input-only, but good practice

    ESP_LOGI(TAG, "Ultrasonic sensor initialized (TRIG: 21, ECHO: 35).");

    // 3. Create the FreeRTOS task
    xTaskCreate(ultrasonic_task, "ultrasonic_monitor", 
                2048,           // Stack size
                NULL,           // Parameters
                5,              // Priority
                NULL);          // Task handle
}
