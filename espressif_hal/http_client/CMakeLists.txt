idf_component_register( SRCS "http_client.cpp"
                        REQUIRES fic_hal fic_log fic_time queue task esp_http_client
                        INCLUDE_DIRS ".")
