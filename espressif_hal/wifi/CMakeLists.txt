idf_component_register(SRCS "wifi_station.cpp" "wifi_scanner.cpp" "wifi_controller.cpp" "wifi_access_point.cpp"
                       REQUIRES fic_hal fic_log esp_wifi
                       INCLUDE_DIRS ".")
