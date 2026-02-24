#include <stdarg.h>


#ifndef FIC_LOGS_HH
#define FIC_LOGS_HH


enum log_level_t {
    FIC_NONE,
    FIC_VERBOSE,
    FIC_DEBUG,
    FIC_INFO,
    FIC_WARNING,
    FIC_ERROR,
    FIC_CRITICAL
};

typedef void (*fic_log_backend_t)(log_level_t level, const char* tag, const char* format, va_list args);

void fic_log_set_backend(fic_log_backend_t backend);

void fic_log_dispatch(log_level_t level, const char* tag, const char* format, ...);

#define FIC_LOGV(tag, format, ...) fic_log_dispatch(FIC_VERBOSE, tag, format, ##__VA_ARGS__)
#define FIC_LOGD(tag, format, ...) fic_log_dispatch(FIC_DEBUG, tag, format, ##__VA_ARGS__)
#define FIC_LOGI(tag, format, ...) fic_log_dispatch(FIC_INFO, tag, format, ##__VA_ARGS__)
#define FIC_LOGW(tag, format, ...) fic_log_dispatch(FIC_WARNING, tag, format, ##__VA_ARGS__)
#define FIC_LOGE(tag, format, ...) fic_log_dispatch(FIC_ERROR, tag, format, ##__VA_ARGS__)
#define FIC_LOGC(tag, format, ...) fic_log_dispatch(FIC_CRITICAL, tag, format, ##__VA_ARGS__)

#endif