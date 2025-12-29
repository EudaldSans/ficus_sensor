idf_component_register(SRCS "temperature_sensors/ds18b20.cpp"
                       REQUIRES adc onewire
                       INCLUDE_DIRS "." "temperature_sensors")
