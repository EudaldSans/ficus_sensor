

#ifndef IN_ERRORS_H
#define IN_ERRORS_H

enum in_error_t {
    IN_OK = 0,
    IN_ERR_FAIL = -1,

    IN_ERR_SDK_FAIL = 0x99,

    IN_ERR_NO_MEM = 0x100,
    IN_ERR_INVALID_ARG = 0x101,
    IN_ERR_INVALID_SIZE = 0x102,
    IN_ERR_INVALID_STATE = 0x103,
    IN_ERR_NOT_FOND = 0x104,
    IN_ERR_NOT_SUPPORTED = 0x105,
    IN_ERR_TIMEOUT = 0x106,
    
    IN_ERR_INVALID_CRC = 0x108,

    IN_ERR_NOT_FINISHED = 0x10c,
    IN_ERR_NOT_ALLOWED = 0x10d,

    IN_ERR_NVS_BASE = 0x1100,

    IN_ERR_ULP_BASE = 0x1200,

    IN_ERR_OTA_BASE = 0x1500,

    IN_ERR_WIFI_BASE = 0x3000
};

#define IN_ERR_RETURN_ON_FALSE(a, err_code, log_action) do {    \
        if (unlikely(!(a))) {                                   \
            log_action;                                         \
            return err_code;                                    \
        }                                                       \
    } while(0)

#define IN_RETURN_VALUE_ON_ERROR(x, value, log_action) do { \
        in_error_t err_rc_ = (x);                           \
        if (unlikely(err_rc_ != IN_OK)) {                  \
            log_action;                                     \
            return value;                                   \
        }                                                   \
    } while(0)

#define IN_RETURN_ON_ERROR(x, log_action) do {  \
        in_error_t err_rc_ = (x);               \
        if (unlikely(err_rc_ != IN_OK)) {       \
            log_action;                         \
            return err_rc_;                     \
        }                                       \
    } while(0)

#define IN_RETURN_VOID_ON_ERROR(x, log_action) do {  \
        in_error_t err_rc_ = (x);               \
        if (unlikely(err_rc_ != IN_OK)) {       \
            log_action;                         \
            return;                             \
        }                                       \
    } while(0)

#endif