idf_component_register(SRCS "ficus_sensor_main.cpp"
                       PRIV_REQUIRES spi_flash sensors channel_linking adc onewire
                       INCLUDE_DIRS ".")
