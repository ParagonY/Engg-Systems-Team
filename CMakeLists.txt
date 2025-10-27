# Sets the minimum required CMake version
cmake_minimum_required(VERSION 3.16)

# Set the name of your project explicitly
set(PROJECT_NAME "Engg-Systems-Team")

# CRITICAL: Explicitly include the main project build system file.
# This ensures environment variables are set before the project command runs.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define the project (languages are implicitly handled by the include above)
project(${PROJECT_NAME})