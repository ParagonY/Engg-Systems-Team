# Registers the 'main' component.
# idf_component_register(
#     # Define the source file(s) for the component.

#     SRCS  "ESPListener.cpp"
#     INCLUDE_DIRS "."

#     "functions.cpp"

    
#     # Dependencies required by the component, including C++ runtime (cxx) 
#     # and all Wi-Fi/Networking components.
#     PRIV_REQUIRES 
#         freertos  
#         log       
#         esp_system
#         cxx 
#         nvs_flash   
#         esp_netif   
#         esp_event
#         esp_wifi   # Changed from 'wifi' to the explicit component name 'esp_wifi'
#         lwip
#         driver      # Needed for driver/gpio.h and ledc.h
#         esp_timer
# )


# CMakeLists.txt for the Smart Bridge Control ESP-IDF Project

# Set the required C++ standard. C++17 is the standard for modern ESP-IDF.
set(CMAKE_CXX_STANDARD 17)

# Register the component, including all source files and dependencies.
idf_component_register(
    # List all C++ source files separated by spaces.
    SRCS "main.cpp" "functions.cpp" 

    # Look for headers in the current directory.
    INCLUDE_DIRS "."

    # Dependencies required by your code: FreeRTOS, Logging, Wi-Fi, Sockets (lwip), and the C++ runtime (cxx).
    PRIV_REQUIRES 
        freertos    
        log         
        esp_system  
        cxx         
        nvs_flash   
        esp_netif   
        esp_event   
        esp_wifi    
        lwip
        driver      # For GPIO and other drivers
        esp_timer
)


# This is the main component's CMakeLists.txt file.

# Register the component, listing ALL source files (.cpp, .c) directly.
# functions.cpp is added here to ensure the linker finds all functions.
# idf_component_register(SRCS
#     "ESPListener.cpp"
#     "functions.cpp" # <--- NEW FILE ADDED HERE
#     PRIV_REQUIRES 
#         driver      # Needed for driver/gpio.h and ledc.h
#         esp_timer   # Needed for microsecond timing
#         freertos    # Needed for tasks and queues
#         lwip        # Needed for TCP/IP networking
# )

