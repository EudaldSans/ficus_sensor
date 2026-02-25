set(base_deps 
    spi_flash 
    fic_time
    esp_fic_time
    task
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
        ${timing}
)
