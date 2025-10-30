# Registers the 'main' component.
idf_component_register(
    # Define the source file(s) for the component.
    SRCS "ESPListener.cpp"
    
    INCLUDE_DIRS "."
    
    # Dependencies required by the component, including C++ runtime (cxx) 
    # and all Wi-Fi/Networking components.
    PRIV_REQUIRES 
        freertos  
        log       
        esp_system
        cxx 
        nvs_flash   
        esp_netif   
        esp_event
        esp_wifi   # Changed from 'wifi' to the explicit component name 'esp_wifi'
        lwip
)