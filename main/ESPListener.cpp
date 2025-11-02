#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>  // POSIX sockets
#include <netdb.h>       
#include <unistd.h>      // For close()

// ESP-IDF specific headers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h" // For waiting on the AP start event
#include "esp_log.h"     
#include "esp_event.h"   
#include "esp_wifi.h"    
#include "nvs_flash.h"   
#include "lwip/ip4_addr.h" 

static const char *TAG = "AP_CLIENT";

// Port and Address Configuration
#define TARGET_SERVER_PORT 5050

// *** CRITICAL CHANGE: TARGET IS NOW THE PC'S FIXED IP ON THE AP NETWORK ***
// When your PC connects to the ESP32's AP, it is usually assigned 192.168.4.2.
#define TARGET_SERVER_IP "192.168.4.2" 
#define BUFFER_SIZE 1024

// --- ACCESS POINT CREDENTIALS ---
// The name and password your PC will use to connect to the ESP32
#define AP_SSID         "ESP32_UI_NETWORK"    
#define AP_PASSWORD     "password123" 
#define MAX_STA_CONN    1 
// ------------------------------------

// Event Group used to synchronize the main application with the AP start
static EventGroupHandle_t wifi_event_group;
const int AP_STARTED_BIT = BIT0; 

// Forward Declaration (Required for C++)
static void tcp_client_task(void *pvParameters);


// --- Wi-Fi Event Handler (Monitors AP events) ---

static void event_handler(void* arg, esp_event_base_t event_base, 
                          int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_AP_START) {
            ESP_LOGI(TAG, "Access Point started successfully.");
            // Signal that the AP is ready
            xEventGroupSetBits(wifi_event_group, AP_STARTED_BIT);
        } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
            ESP_LOGI(TAG, "PC connected to Access Point.");
        } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
            ESP_LOGI(TAG, "PC disconnected from Access Point.");
        }
    }
}

// --- ACCESS POINT Initialization Logic ---

void wifi_init_ap(void) {
    wifi_event_group = xEventGroupCreate();
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Create the default AP network interface (sets IP to 192.168.4.1)
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    // Configure the Access Point settings
    wifi_config_t wifi_config = {}; 
    
    // Set SSID, Password, and Max connections
    strcpy((char*)wifi_config.ap.ssid, AP_SSID);
    strcpy((char*)wifi_config.ap.password, AP_PASSWORD);
    wifi_config.ap.ssid_len = strlen(AP_SSID);
    wifi_config.ap.max_connection = MAX_STA_CONN;
    
    // Open WPA2/WPA3 Personal (for basic security)
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_WPA3_PSK; 
    
    // Set mode and configuration
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Access Point creation initiated. SSID: %s, IP: 192.168.4.1", AP_SSID);
    
    // Wait for the AP to fully start before proceeding
    xEventGroupWaitBits(wifi_event_group, AP_STARTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    ESP_LOGI(TAG, "Access Point ready. Please connect your PC to '%s'.", AP_SSID);
}


// --- Main Client Task (Runs repeatedly to try connecting to the PC Server) ---

static void tcp_client_task(void *pvParameters) {
    // Wait a moment for the PC to potentially connect to the AP
    vTaskDelay(pdMS_TO_TICKS(5000)); 
    
    while (1) {
        // --- FIXED: Moved all variable declarations to the top of the scope ---
        struct addrinfo hints = {}; 
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        struct addrinfo *res = NULL;
        int s = -1;
        
        char port_str[6];
        sprintf(port_str, "%d", TARGET_SERVER_PORT);
        
        // Communication variables declared here to avoid 'jump crosses initialization' error
        const char* message = "Hello from the ESP32 AP Client!\n"; 
        char recvBuffer[BUFFER_SIZE];
        int bytesSent;
        int bytesReceived;
        
        // ------------------------------------------------------------------------

        ESP_LOGI(TAG, "Attempting connection to PC Server: %s:%d", TARGET_SERVER_IP, TARGET_SERVER_PORT);
        
        int err = getaddrinfo(TARGET_SERVER_IP, port_str, &hints, &res);

        if (err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed for 192.168.4.2 err=%d", err);
            goto cleanup;
        }
        
        // 1. Create the Socket
        s = socket(res->ai_family, res->ai_socktype, 0);
        if (s < 0) {
            ESP_LOGE(TAG, "Failed to create socket: errno %d", errno);
            goto cleanup;
        }

        // 2. Connect to the PC Server (192.168.4.2)
        if (connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGW(TAG, "Connection failed. Server not yet ready? errno %d", errno);
            goto cleanup;
        }
        
        ESP_LOGI(TAG, "Successfully connected to PC Server!");
        
        // 3. Send and Receive Data (variables were declared above)
        
        // Sending data
        bytesSent = send(s, message, strlen(message), 0);
        if (bytesSent < 0) {
            ESP_LOGE(TAG, "send failed: errno %d", errno);
        } else {
            ESP_LOGI(TAG, "Sent %d bytes: %s", bytesSent, message);
        }
        
        // Receiving data (Wait for a response)
        bytesReceived = recv(s, recvBuffer, BUFFER_SIZE - 1, 0);
        
        if (bytesReceived > 0) {
            recvBuffer[bytesReceived] = '\0'; 
            ESP_LOGI(TAG, "Received %d bytes: %s", bytesReceived, recvBuffer);
        } else if (bytesReceived == 0) {
            ESP_LOGW(TAG, "Connection closed by PC Server.");
        } else {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
        }
        
    cleanup:
        // if (s != -1) {
        //     close(s);
        // }
        // if (res != NULL) {
        //     freeaddrinfo(res);
        // }

        // Wait 10 seconds before trying again (if disconnected or failed)
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

    // This line is technically unreachable but kept for convention
    vTaskDelete(NULL); 
}


// --- Main ESP-IDF Entry Point ---

extern "C" void app_main(void) {
    // Initialize NVS (required for Wi-Fi)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP32 AP mode starting up...");
    
    // 1. Initialize Access Point
    wifi_init_ap();
    
    // 2. Start the TCP Client Task (it will run repeatedly)
    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
}