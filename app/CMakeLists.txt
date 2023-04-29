cmake_minimum_required(VERSION 3.5)
project(access_node)

set(CMAKE_CXX_STANDARD 11)

find_package(PahoMqttCpp REQUIRED)

# Add the sss source files
set(SSS_SOURCE_FILES
    sss/sss.c
    sss/hazmat.c
    sss/randombytes.c
    sss/tweetnacl.c
)

add_executable(access_node access_node.cpp mqtt.cpp rsa.cpp shamir_secret_sharing.cpp ${SSS_SOURCE_FILES})
target_link_libraries(access_node PahoMqttCpp::paho-mqttpp3)

# Include the sss directory for header files
target_include_directories(access_node PRIVATE sss)
