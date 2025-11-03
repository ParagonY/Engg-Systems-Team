# Sets the minimum required CMake version
cmake_minimum_required(VERSION 3.16)

# Set the name of your project explicitly
set(PROJECT_NAME "Engg-Systems-Team")

# CRITICAL: Explicitly include the main project build system file.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define the project
project(${PROJECT_NAME})


# idf_component_register(SRCS "motor.cpp" "sensor-motor.cpp" "main.cpp" "functions.cpp"
#                        INCLUDE_DIRS ".")