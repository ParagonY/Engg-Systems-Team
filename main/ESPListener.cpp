// #include <iostream>
// #include <string>
// #include <memory>
// #include <cstring>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>

// constexpr int PORT = 8080;
// constexpr int BUFFER_SIZE = 1024;
// int main() {
//     int sock = 0;
//     struct sockaddr_in serv_addr;
//     char buffer[BUFFER_SIZE] = {0};
//     // Creating socket file descriptor
//     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         std::cerr << "Socket creation error" << std::endl;
//         return -1;
//     }
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(PORT);
//     // Convert IPv4 and IPv6 addresses from text to binary form
//     if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
//         std::cerr << "Invalid address/ Address not supported" << std::endl;
//         return -1;
//     }
//     // Connect to the server
//     if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
//         std::cerr << "Connection Failed" << std::endl;
//         return -1;
//     }
//     std::string hello = "Hello from client";
//     send(sock, hello.c_str(), hello.size(), 0);
//     std::cout << "Hello message sent" << std::endl;
//     ssize_t valread = read(sock, buffer, BUFFER_SIZE);
//     std::cout << "Received: " << buffer << std::endl;
//     // Close the socket
//     close(sock);
//     return 0;
// }

// #include <iostream>

// int main() {

//     std::cout << "Hello, World!" << "\n";

//     return 0;
// }

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// **********************************************************
// CRITICAL: The extern "C" wrapper is mandatory for C++ main.cpp
// to tell the C++ compiler to compile and link this function 
// using C language conventions, which FreeRTOS expects.
extern "C" {
    void app_main(void);
}
// **********************************************************

// The main entry point for the ESP32 application.
void app_main(void)
{
    // Define a tag for logging output
    static const char *TAG = "LISTENER_APP";
    
    // Log a message to the console
    ESP_LOGI(TAG, "C++ Application Started Successfully! Starting listener loop...");
    
    int count = 0;
    while (1) {
        ESP_LOGI(TAG, "Listener Running. Count: %d", count++);
        vTaskDelay(pdMS_TO_TICKS(3000)); // Delay for 3 seconds
    }
}
