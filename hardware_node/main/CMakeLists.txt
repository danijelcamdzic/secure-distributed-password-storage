file(GLOB MQTT_SOURCES "../mqtt311-client-library/MQTT311Client/*.c")
file(GLOB RSA_SOURCES "../RSA/*.c")

idf_component_register(SRCS "main.c"
                            "tcp_functions.c"
                            "ssl_functions.c"
                            "mqtt_functions.c"
                            "nvs_functions.c"
                            ${MQTT_SOURCES}
                            ${RSA_SOURCES}
                    INCLUDE_DIRS "." "../mqtt311-client-library/MQTT311Client" "../mqtt311-client-library" "../RSA" "..")
