idf_component_register(SRCS "onewire_bus.cpp"
                       REQUIRES espressif__onewire_bus fic_hal fic_log
                       INCLUDE_DIRS ".")
