idf_component_register(SRCS "wifi_driver.cpp"
                       REQUIRES fic_hal fic_log esp_wifi nvs_flash
                       INCLUDE_DIRS ".")
