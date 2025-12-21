idf_component_register(SRCS "adc.c" "hello_world_main.c" "ds18b20.c" "adc.c"
                       PRIV_REQUIRES spi_flash esp_adc
                       INCLUDE_DIRS ".")
