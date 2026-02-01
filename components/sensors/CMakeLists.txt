idf_component_register(SRCS "sensor.cpp"
                            "temperature_sensors/ds18b20.cpp"
                            "humidity_sensors/analog_humidity_sensor.cpp"
                       REQUIRES adc onewire channel_linking
                       INCLUDE_DIRS "." "temperature_sensors" "humidity_sensors")
