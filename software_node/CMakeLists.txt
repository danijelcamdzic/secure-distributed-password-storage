# This CMakeLists.txt file configures the build system for an "access_node" 
# project that uses C++17 and depends on the PahoMqttCpp library. It specifies 
# the main source files and the Shamir's Secret Sharing (sss) source files for 
# the project, creates an executable target, and sets up the include directories 
# for the target.

# Specify the minimum required version of CMake for this project
cmake_minimum_required(VERSION 3.5)

# Declare a new project named "access_node"
project(access_node)

# Set the C++ standard to C++17 for this project
set(CMAKE_CXX_STANDARD 17)

# Uncomment the following line to enable debug mode
add_definitions(-DDEBUG)

# Find the PahoMqttCpp package and make it a required dependency
find_package(PahoMqttCpp REQUIRED)

# Create a list of main source files for the access_node project
set(MAIN_SOURCE_FILES
    main/main.cpp
    main/mqtt_functions.cpp
    main/rsa_functions.cpp
    main/shamir_secret_sharing_functions.cpp
)

# Create a list of sss (Shamir's Secret Sharing) source files for the access_node project
set(SSS_SOURCE_FILES
    sss/sss.c
    sss/hazmat.c
    sss/randombytes.c
    sss/tweetnacl.c
)

# Add an executable target called "access_node" that is built from the specified source files
add_executable(access_node ${MAIN_SOURCE_FILES} ${SSS_SOURCE_FILES})

# Link the PahoMqttCpp library to the "access_node" target
target_link_libraries(access_node PahoMqttCpp::paho-mqttpp3)

# Add the "main" directory to the include path for the "access_node" target
target_include_directories(access_node PRIVATE main)

# Add the "sss" directory to the include path for the "access_node" target
target_include_directories(access_node PRIVATE sss)

