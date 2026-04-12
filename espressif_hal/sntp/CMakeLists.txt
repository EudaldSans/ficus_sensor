idf_component_register(SRCS "sntp_client.cpp"
                       REQUIRES fic_hal fic_log esp_netif lwip
                       INCLUDE_DIRS ".")
