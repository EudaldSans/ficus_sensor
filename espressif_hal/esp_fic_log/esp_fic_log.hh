#include "fic_log.hh"


#ifndef ESP_FIC_LOG_HH
#define ESP_FIC_LOG_HH

void esp32_backend(log_level_t level, const char* tag, const char* format, va_list args);

#endif
