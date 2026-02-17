idf_component_register(SRCS "temperature_sensors/ds18b20.cpp"
                            "humidity_sensors/analog_humidity_sensor.cpp"
                            "sensor_endpoint_factory.cpp"
                       REQUIRES fic_hal channel_linking threading
                       INCLUDE_DIRS "." "temperature_sensors" "humidity_sensors")
