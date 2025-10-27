# Registers the 'main' component.
idf_component_register(
    # CRITICAL: Define the source file(s) for the component.
    SRCS "ESPListener.cpp"
    
    INCLUDE_DIRS "."
    
    # Dependencies required by the component, including C++ runtime (cxx).
    # This automatically fixes the 'undefined reference to app_main' error.
    PRIV_REQUIRES 
        freertos  
        log       
        esp_system
        cxx 
)

# NOTE: The redundant 'target_link_libraries(${COMPONENT_NAME} PRIVATE cxx)' line 
# has been removed, as it caused a CMake error by trying to link the same library twice.
