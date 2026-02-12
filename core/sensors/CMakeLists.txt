idf_component_register(SRCS "sensor_endpoints.cpp"
                            "temperature_sensors/ds18b20.cpp"
                            "humidity_sensors/analog_humidity_sensor.cpp"
                       REQUIRES adc onewire channel_linking threading
                       INCLUDE_DIRS "." "temperature_sensors" "humidity_sensors")
