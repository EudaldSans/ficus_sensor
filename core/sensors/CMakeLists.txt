idf_component_register(SRCS "temperature_sensors/ds18b20.cpp"
                            "humidity_sensors/analog_humidity_sensor.cpp"
                       REQUIRES fic_hal fic_time
                       INCLUDE_DIRS "." "temperature_sensors" "humidity_sensors")
