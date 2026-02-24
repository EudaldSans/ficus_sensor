idf_component_register(SRCS "ficus_sensor_main.cpp"
                       PRIV_REQUIRES spi_flash sensors sensor_adapters channel_linking adc onewire leds signalling fic_log esp_fic_log
                       INCLUDE_DIRS ".")
