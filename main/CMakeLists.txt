set(base_deps 
    spi_flash 
    fic_time
    esp_fic_time
    task
    queue
)

set(stacks
    firebase_stack
)

set(hardware_deps 
    adc 
    onewire
    leds
    nvs_flash
)

set(communications
    wifi
    http_client
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
    versioning
)

idf_component_register(
    SRCS "ficus_sensor_main.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
        ${base_deps}
        ${stacks}
        ${hardware_deps}
        ${communications}
        ${core_logic}
        ${sensor_logic}
        ${logging}
        ${timing}
    EMBED_TXTFILES root_cert.pem
)
