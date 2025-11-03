#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>  
#include <netdb.h>       
#include <unistd.h>     
#include <string>        

// ESP-IDF specific headers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"      
#include "freertos/event_groups.h" 
#include "esp_log.h"     
#include "esp_event.h"   
#include "esp_wifi.h"    
#include "nvs_flash.h"   
#include "lwip/ip4_addr.h" 

static const char *TAG = "AP_CLIENT";

// Port and Address Configuration
#define TARGET_SERVER_PORT 5141
#define TARGET_SERVER_IP "192.168.4.2" 
#define BUFFER_SIZE 8192

// --- ACCESS POINT CREDENTIALS ---
#define AP_SSID         "ESP32_UI_NETWORK"    
#define AP_PASSWORD     "password123" 
#define MAX_STA_CONN    1 
// ------------------------------------

// Event Group used to synchronize the main application with the AP start
static EventGroupHandle_t wifi_event_group;
const int AP_STARTED_BIT = BIT0; 

// --- GLOBAL COMMUNICATION HANDLES ---
// 1. Queue: Used to pass received commands from the Listener Task to the Processor Task.
static QueueHandle_t rx_command_queue = NULL;

// 2. Socket Descriptor: Used by the Listener Task to receive and the Processor Task to send replies.
static int g_socket_fd = -1; 
// ------------------------------------


// Forward declarations for the three tasks
static void tcp_listener_task(void *pvParameters); 
static void command_processor_task(void *pvParameters); 
static void hardware_monitor_task(void *pvParameters); 


// --- Wi-Fi Event Handler (Monitors AP events) ---
static void event_handler(void* arg, esp_event_base_t event_base, 
                          int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_AP_START) {
            ESP_LOGI(TAG, "Access Point started successfully.");
            xEventGroupSetBits(wifi_event_group, AP_STARTED_BIT);
        } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
            ESP_LOGI(TAG, "PC connected to Access Point.");
        } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
            ESP_LOGW(TAG, "PC disconnected from Access Point. Connection will fail.");
        }
    }
}

// --- ACCESS POINT Initialization Logic ---
void wifi_init_ap(void) {
    wifi_event_group = xEventGroupCreate();
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {}; 
    strcpy((char*)wifi_config.ap.ssid, AP_SSID);
    strcpy((char*)wifi_config.ap.password, AP_PASSWORD);
    wifi_config.ap.ssid_len = strlen(AP_SSID);
    wifi_config.ap.max_connection = MAX_STA_CONN;
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_WPA3_PSK; 
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Access Point creation initiated. SSID: %s, IP: 192.168.4.1", AP_SSID);
    xEventGroupWaitBits(wifi_event_group, AP_STARTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG, "Access Point ready. Please connect your PC to '%s'.", AP_SSID);
}


// --- TASK 3: Hardware Monitor Task (Independent Local Control/HW Checks) ---
static void hardware_monitor_task(void *pvParameters) {
    // NOTE: This task represents your continuous local hardware monitoring and safety checks.
    
    // Placeholder for hardware initialization (e.g., setup_pins();)
    ESP_LOGI("HW_MONITOR", "Hardware initialized. Starting local checks...");
    
    while (1) {
        ESP_LOGI("HW_MONITOR", "Hello, C++! Running theoretical hardware code.");
        // Placeholder for running continuous local logic (e.g., bridge_sequence();)
        ESP_LOGD("HW_MONITOR", "Running local safety checks and sensor readings.");
        
        // This task runs completely independently of network traffic.
        //vTaskDelay(pdMS_TO_TICKS(1500)); 
    }
}


// --- TASK 2: Command Processor Task (Consumer, Dispatcher, and Sender) ---
static void command_processor_task(void *pvParameters) {
    char* received_command_ptr;
    
    while (1) {
        // Try to receive a command from the queue (wait for a short period)
        if (xQueueReceive(rx_command_queue, &received_command_ptr, pdMS_TO_TICKS(100)) == pdPASS) {
            
            ESP_LOGI("PROCESSOR", "==> Processing Command: %s", received_command_ptr);
            
            // --- 1. PROCESS RECEIVED COMMAND ---
            const char* reply_message = "ACK: UNKNOWN_COMMAND\n";

            if (strcmp(received_command_ptr, "OPEN_BRIDGE") == 0) {
                ESP_LOGW("PROCESSOR", "Action: Initiating bridge opening sequence!");
                // You would call your hardware control function here (e.g., motor_control(true))
                reply_message = "ACK: BRIDGE_OPENING\n";
            } else if (strcmp(received_command_ptr, "STATUS_REQUEST") == 0) {
                ESP_LOGI("PROCESSOR", "Action: Gathering status data.");
                // You would get real status data from your HW and format it here
                reply_message = "STATUS: BRIDGE_CLOSED\n"; 
            }
            
            // --- 2. SEND A REPLY MESSAGE (if socket is active) ---
            if (g_socket_fd != -1) {
                send(g_socket_fd, reply_message, strlen(reply_message), 0);
                ESP_LOGI("PROCESSOR", "Sent reply: %s", reply_message);
            } else {
                ESP_LOGW("PROCESSOR", "Cannot send reply, socket is currently closed/reconnecting.");
            }
            
            // 3. Free the memory allocated by the Listener Task
            free(received_command_ptr);
            // --------------------------------------------------------

        } else {
            // No command received this cycle. Wait for the next one.
            vTaskDelay(pdMS_TO_TICKS(50)); 
        }
    }
}


// --- TASK 1: TCP Listener Task (Network Handler and Producer) ---
static void tcp_listener_task(void *pvParameters) {
    // This outer loop handles connection retries if the persistent connection breaks.
    while (1) { 
        struct addrinfo hints = {}; 
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        struct addrinfo *res = NULL;
        int s = -1;
        
        char port_str[6];
        sprintf(port_str, "%d", TARGET_SERVER_PORT);
        
        char recvBuffer[BUFFER_SIZE];
        int bytesReceived;

        ESP_LOGI(TAG, "Attempting connection to PC Server: %s:%d", TARGET_SERVER_IP, TARGET_SERVER_PORT);
        
        // Connection setup logic (omitted for brevity)
        int err = getaddrinfo(TARGET_SERVER_IP, port_str, &hints, &res);
        if (err != 0 || res == NULL) { goto retry; }
        s = socket(res->ai_family, res->ai_socktype, 0);
        if (s < 0) { goto cleanup; }
        if (connect(s, res->ai_addr, res->ai_addrlen) != 0) { goto cleanup; }
        
        ESP_LOGI(TAG, "Successfully connected to PC Server and keeping connection open!");
        
        // SET GLOBAL SOCKET (Allows the Processor Task to send replies)
        g_socket_fd = s;
        
        // 4. PERSISTENT COMMUNICATION LOOP (Listening for Commands)
        while (1) {
            // Set a timeout so we don't block forever, allowing loop to check for errors/status
            struct timeval timeout = { .tv_sec = 2, .tv_usec = 0 };
            setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

            bytesReceived = recv(s, recvBuffer, BUFFER_SIZE - 1, 0);
            
            if (bytesReceived > 0) {
                recvBuffer[bytesReceived] = '\0'; 
                ESP_LOGI(TAG, "Received %d bytes: %s", bytesReceived, recvBuffer);

                // --- ADD RECEIVED MESSAGE TO QUEUE (The Producer) ---
                char* command_copy = (char*)malloc(bytesReceived + 1);
                if (command_copy) {
                    strcpy(command_copy, recvBuffer);

                    // Send the pointer to the command copy to the queue
                    if (xQueueSend(rx_command_queue, (void*)&command_copy, pdMS_TO_TICKS(10)) != pdPASS) {
                        ESP_LOGE(TAG, "Queue full! Dropping command and freeing memory.");
                        free(command_copy); 
                    }
                }
                // --------------------------------------------------------

            } else if (bytesReceived == 0) {
                ESP_LOGW(TAG, "Connection closed gracefully by PC Server.");
                break; // Exit communication loop to trigger cleanup/retry
            } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                // Hard error occurred
                ESP_LOGE(TAG, "recv failed: errno %d. Connection lost.", errno);
                break; // Exit communication loop to trigger cleanup/retry
            } 

            // Allow other tasks to run
            vTaskDelay(pdMS_TO_TICKS(100)); 
        }

    cleanup:
        // Connection broken or error occurred. Clean up socket and reset global flag.
        if (s != -1) { 
            close(s); 
            g_socket_fd = -1; // Reset global socket handle
            s = -1; 
            ESP_LOGI(TAG, "Socket closed and global handle reset.");
        }
        if (res != NULL) {
            freeaddrinfo(res);
        }

    retry:
        ESP_LOGI(TAG, "Waiting 10s before attempting connection retry...");
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

    vTaskDelete(NULL); 
}


// --- Main ESP-IDF Entry Point (The Initializer/Starter Thread) ---

extern "C" void app_main(void) {
    // 0. Initialize NVS and Wi-Fi
    esp_err_t ret = nvs_flash_init();
    // (Error checking and Wi-Fi init omitted for brevity but present)
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP32 AP mode starting up...");
    wifi_init_ap();
    
    // 1. CREATE THE QUEUE (The central buffer for network commands)
    rx_command_queue = xQueueCreate(5, sizeof(char*));
    if (rx_command_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create command queue! System halt.");
        return; 
    }

    // 2. Start the three concurrent tasks
    
    // Task 1: TCP Listener (Connects, Listens, Produces commands to queue)
    xTaskCreate(tcp_listener_task, "listener", 4096, NULL, 5, NULL);
    
    // Task 2: Command Processor (Consumes from queue, processes, sends replies)
    xTaskCreate(command_processor_task, "processor", 4096, NULL, 5, NULL);
    
    // Task 3: Hardware Monitor (Independent loop for local logic and safety checks)
    xTaskCreate(hardware_monitor_task, "hw_monitor", 4096, NULL, 4, NULL);
    
    // app_main returns, and the scheduler takes over running the three tasks.
}