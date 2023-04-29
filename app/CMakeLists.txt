cmake_minimum_required(VERSION 3.5)
project(mqtt_subscriber)

set(CMAKE_CXX_STANDARD 11)

find_package(PahoMqttCpp REQUIRED)

add_executable(mqtt_subscriber mqtt_subscriber.cpp)
target_link_libraries(mqtt_subscriber PahoMqttCpp::paho-mqttpp3)

