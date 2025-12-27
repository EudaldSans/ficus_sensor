idf_component_register(SRCS "ficus_sensor_main.cpp" "ds18b20.c"
                       PRIV_REQUIRES spi_flash adc onewire
                       INCLUDE_DIRS ".")
