# # Registers the 'main' component.
# idf_component_register(
#     # Define the source file(s) for the component.
#     SRCS "ESPListener.cpp"
    
#     INCLUDE_DIRS "."
    
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
# )



# This is the main component's CMakeLists.txt file.

# Register the component, listing source files and dependencies directly.
# We now include only ESPListener.cpp as it contains all application and hardware code.
idf_component_register(SRCS
    "ESPListener.cpp"
    PRIV_REQUIRES 
        driver      # Needed for driver/gpio.h
        esp_timer   # Needed for microsecond timing
        freertos    # Needed for tasks and queues
        lwip        # Needed for TCP/IP networking
)


