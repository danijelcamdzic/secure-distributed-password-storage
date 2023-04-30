cmake_minimum_required(VERSION 3.5)
project(access_node)

set(CMAKE_CXX_STANDARD 11)

find_package(PahoMqttCpp REQUIRED)

# Add the main source files
set(MAIN_SOURCE_FILES
    main/main.cpp
    main/mqtt.cpp
    main/rsa.cpp
    main/shamir_secret_sharing.cpp
)

# Add the sss source files
set(SSS_SOURCE_FILES
    sss/sss.c
    sss/hazmat.c
    sss/randombytes.c
    sss/tweetnacl.c
)

add_executable(access_node ${MAIN_SOURCE_FILES} ${SSS_SOURCE_FILES})
target_link_libraries(access_node PahoMqttCpp::paho-mqttpp3)

# Include the sss directory for header files
target_include_directories(access_node PRIVATE main)
target_include_directories(access_node PRIVATE sss)
