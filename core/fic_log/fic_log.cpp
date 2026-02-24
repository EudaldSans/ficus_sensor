
#include "fic_log.hh"

static fic_log_backend_t current_backend = nullptr;

/**
 * @brief Sets the log backend with the end points for the fic logs
 * 
 * @param backend @c fic_log_backend_t function pointer to the backend function to be used for log dispatching
 */
void fic_log_set_backend(fic_log_backend_t backend) {
    current_backend = backend;
}

/**
 * @brief Creates variable argument list and dispatches it to current backend function
 * 
 * @param level @c log_level_t with the level for the log message
 * @param tag pointer to const char with the tag for the log message
 * @param format Log format string
 * @param ... Variable arguments for the log message
 */
void fic_log_dispatch(log_level_t level, const char* tag, const char* format, ...) {
    if (current_backend) {
        va_list args;
        va_start(args, format);
        current_backend(level, tag, format, args);
        va_end(args);
    }
}