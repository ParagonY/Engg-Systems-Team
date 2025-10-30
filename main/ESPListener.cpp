// // --- Winsock TCP Client for Windows ---
// // Connects to localhost (127.0.0.1) on port 5050.
// // This requires linking with Ws2_32.lib.

// #include <iostream>
// #include <cstdlib>
// #include <winsock2.h> // Core Winsock functions
// #include <ws2tcpip.h> // Needed for address structures

// // Port and Address Configuration
// #define PORT 5050
// #define IP_ADDRESS "127.0.0.1"
// #define BUFFER_SIZE 1024

// // Link to the Winsock library (required for compilation)
// #pragma comment(lib, "Ws2_32.lib")

// // --- Custom Error Handling Function ---
// // Prints an error message, the specific Winsock error code, cleans up Winsock, and exits.
// void ErrExit(const char* msg) {
//     std::cerr << "--- FATAL ERROR ---" << std::endl;
//     std::cerr << msg << std::endl;
    
//     // Get and print the last specific Winsock error
//     int wsaError = WSAGetLastError();
//     std::cerr << "Winsock Error Code: " << wsaError << std::endl;
    
//     WSACleanup(); // Always clean up before exiting on error
//     exit(EXIT_FAILURE); 
// }
// int main(){
//     // 1. Initialize Winsock
//     WSADATA wsaData;
//     std::cout << "Starting Winsock initialization..." << std::endl;
    
//     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//         // This is where your previous syntax error occurred; now handled by ErrExit
//         ErrExit("WSAStartup() failed!"); 
//     }
//     std::cout << "Winsock initialized successfully." << std::endl;

//     // 2. Create the Client Socket
//     SOCKET clientSocket = INVALID_SOCKET;
//     // AF_INET: IPv4 protocol family
//     // SOCK_STREAM: TCP stream socket (connection-oriented)
//     // IPPROTO_TCP: Protocol
//     clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
//     if (clientSocket == INVALID_SOCKET) {
//         ErrExit("socket() creation failed!");
//     }
//     std::cout << "Socket created." << std::endl;

//     // 3. Define the Server Address Structure
//     struct sockaddr_in serverAddr;
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(PORT); // htons converts port number to network byte order
    
//     // Convert IP address string to binary form
//     // Note: inet_addr is generally deprecated, but used here for simplicity.
//     // Recommended modern function is InetPton, but it requires a slightly different setup.
//     serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

//     // Check if IP address conversion succeeded
//     if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
//         ErrExit("Invalid IP address format.");
//     }

//     // 4. Connect to the Server
//     std::cout << "Attempting connection to " << IP_ADDRESS << ":" << PORT << "..." << std::endl;
    
//     if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
//         ErrExit("connect() failed. Ensure a server is running on the specified port.");
//     }
//     std::cout << "Successfully connected to the server!" << std::endl;

//     // 5. Send and Receive Data
//     const char* message = "Hello from the C++ Winsock client!\n";
//     char recvBuffer[BUFFER_SIZE] = {0};
//     int bytesSent;
//     int bytesReceived;

//     // Sending data
//     bytesSent = send(clientSocket, message, (int)strlen(message), 0);
//     if (bytesSent == SOCKET_ERROR) {
//         ErrExit("send() failed.");
//     }
//     std::cout << "Sent: " << message << std::endl;

//     // Receiving data (Wait for a response)
//     bytesReceived = recv(clientSocket, recvBuffer, BUFFER_SIZE - 1, 0);
//     if (bytesReceived > 0) {
//         // Null-terminate the received data to treat it as a string
//         recvBuffer[bytesReceived] = '\0'; 
//         std::cout << "Received: " << recvBuffer << std::endl;
//     } else if (bytesReceived == 0) {
//         std::cout << "Connection closed by server." << std::endl;
//     } else {
//         ErrExit("recv() failed.");
//     }

//     // 6. Cleanup
//     std::cout << "\nCleaning up..." << std::endl;
    
//     // Close the socket
//     if (closesocket(clientSocket) == SOCKET_ERROR) {
//         std::cerr << "Warning: closesocket() failed with error " << WSAGetLastError() << std::endl;
//     }
    
//     // Clean up Winsock
//     WSACleanup();
    
//     std::cout << "Client program finished successfully." << std::endl;
//     return 0;
// }


// --- ESP-IDF POSIX TCP Client for ESP32 ---
// Connects to a target server after connecting to Wi-Fi.
// --- ESP-IDF POSIX TCP Client for ESP32 ---
// Connects to a target server after connecting to Wi-Fi.

#include <iostream>
#include <cstring> // Needed for strcpy
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>  // POSIX sockets
#include <netdb.h>       // For address resolution (getaddrinfo, struct addrinfo)
#include <unistd.h>      // For close()

// ESP-IDF specific headers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"     // ESP32 logging macros
#include "esp_event.h"   // Event handling
#include "esp_wifi.h"    // Wi-Fi functions
#include "nvs_flash.h"   // Non-volatile storage (for Wi-Fi config)
#include "lwip/ip4_addr.h" // For ip4addr_ntoa

// Tag for ESP-IDF logging
static const char *TAG = "TCP_CLIENT";

// Port and Address Configuration
#define TARGET_SERVER_PORT 5050
#define TARGET_SERVER_IP "10.126.252.150" // <-- CHANGE THIS IP to your server's IP
#define BUFFER_SIZE 1024

// --- HARDCODED WI-FI CREDENTIALS ---
// *** YOU MUST CHANGE THESE VALUES ***
#define WIFI_SSID      "Macquarie OneNet"    
#define WIFI_PASSWORD  "" // Open network
// ------------------------------------


// --- FIX 1: FORWARD DECLARATION ---
// This resolves the "tcp_client_task was not declared" error (Error 1).
static void tcp_client_task(void *pvParameters);


// --- Wi-Fi Initialization and Connection Logic ---

static void event_handler(void* arg, esp_event_base_t event_base, 
                          int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Retrying connection...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP address: %s", ip4addr_ntoa((const ip4_addr_t *)&event->ip_info.ip));
        // Wi-Fi is connected, we can now start the client task
        xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
    }
}

void wifi_init_sta(void) {
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    // --- FIX 2: Structure Initialization for C++ ---
    wifi_config_t wifi_config = {}; // Zero-initialize the entire struct
    
    // Use strcpy to set SSID and Password, avoiding complex designated initializer syntax (Error 2)
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASSWORD);
    
    // Set the authentication mode directly
    wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN; 
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi initialization finished. Using open credentials.");
}


// --- Main Client Task (Runs after Wi-Fi is connected) ---

static void tcp_client_task(void *pvParameters) {
    // 1. Setup Address Information (using getaddrinfo is the robust way)
    // --- FIX 3: Zero-initialize struct addrinfo ---
    // This resolves the warnings about missing initializers.
    struct addrinfo hints = {}; 
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    struct addrinfo *res;
    int s; // Removed unused variable 'r'
    
    char port_str[6];
    sprintf(port_str, "%d", TARGET_SERVER_PORT);

    ESP_LOGI(TAG, "Resolving address %s:%d...", TARGET_SERVER_IP, TARGET_SERVER_PORT);
    int err = getaddrinfo(TARGET_SERVER_IP, port_str, &hints, &res);

    if (err != 0 || res == NULL) {
        ESP_LOGE(TAG, "DNS lookup failed err=%d", err);
        vTaskDelete(NULL);
        return;
    }
    
    // 2. Create the Socket
    s = socket(res->ai_family, res->ai_socktype, 0);
    if (s < 0) {
        ESP_LOGE(TAG, "Failed to create socket: errno %d", errno);
        freeaddrinfo(res);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Socket created.");

    // 3. Connect to the Server
    ESP_LOGI(TAG, "Attempting connection to server...");
    if (connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "Socket connection failed: errno %d", errno);
        close(s);
        freeaddrinfo(res);
        vTaskDelete(NULL);
        return;
    }
    freeaddrinfo(res);
    ESP_LOGI(TAG, "Successfully connected to server!");
    
    // 4. Send and Receive Data
    const char* message = "Hello from the ESP32 Client!\n"; 
    char recvBuffer[BUFFER_SIZE];
    int bytesSent;
    int bytesReceived;

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
        // Null-terminate the received data to treat it as a string
        recvBuffer[bytesReceived] = '\0'; 
        ESP_LOGI(TAG, "Received %d bytes: %s", bytesReceived, recvBuffer);
    } else if (bytesReceived == 0) {
        ESP_LOGW(TAG, "Connection closed by server.");
    } else {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
    }

    // 5. Cleanup
    ESP_LOGI(TAG, "Closing socket and task...");
    
    // Close the socket (POSIX standard)
    close(s);
    
    // Task cleanup
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

    ESP_LOGI(TAG, "ESP32 TCP Client starting up...");
    
    // Start Wi-Fi initialization and connection
    wifi_init_sta();
}
