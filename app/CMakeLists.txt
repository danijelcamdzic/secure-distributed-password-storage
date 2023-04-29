cmake_minimum_required(VERSION 3.5)
project(mqtt_subscriber)

set(CMAKE_CXX_STANDARD 11)

find_package(PahoMqttCpp REQUIRED)

# Add the sss source files
set(SSS_SOURCE_FILES
    sss/sss.c
    sss/hazmat.c
    sss/randombytes.c
    sss/tweetnacl.c
)

add_executable(mqtt_subscriber mqtt_subscriber.cpp ${SSS_SOURCE_FILES})
target_link_libraries(mqtt_subscriber PahoMqttCpp::paho-mqttpp3)

# Include the sss directory for header files
target_include_directories(mqtt_subscriber PRIVATE sss)
