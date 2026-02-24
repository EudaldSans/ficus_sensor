set(base_deps 
    spi_flash 
)

set(hardware_deps 
    adc 
    onewire
    leds
)

set(sensor_logic 
    sensors 
    sensor_adapters 
)

set(core_logic
    signalling
    channel_linking
)

set(logging 
    fic_log 
    esp_fic_log
)

idf_component_register(
    SRCS "ficus_sensor_main.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
        ${base_deps}
        ${hardware_deps}
        ${core_logic}
        ${sensor_logic}
        ${logging}
)


idf_component_register(SRCS "ficus_sensor_main.cpp"
                       PRIV_REQUIRES spi_flash sensors sensor_adapters channel_linking adc onewire leds signalling fic_log esp_fic_log
                       INCLUDE_DIRS ".")
