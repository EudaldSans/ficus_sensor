

#ifndef FIC_ERRORS_HH
#define FIC_ERRORS_HH

enum fic_error_t {
    FIC_OK = 0,
    FIC_ERR_FAIL = -1,

    FIC_ERR_SDK_FAIL = 0x99,

    FIC_ERR_NO_MEM = 0x100,
    FIC_ERR_INVALID_ARG = 0x101,
    FIC_ERR_INVALID_SIZE = 0x102,
    FIC_ERR_INVALID_STATE = 0x103,
    FIC_ERR_NOT_FOUND = 0x104,
    FIC_ERR_NOT_SUPPORTED = 0x105,
    FIC_ERR_TIMEOUT = 0x106,
    
    FIC_ERR_INVALID_CRC = 0x108,

    FIC_ERR_NOT_FINISHED = 0x10c,
    FIC_ERR_NOT_ALLOWED = 0x10d,

    FIC_ERR_NVS_BASE = 0x1100,

    FIC_ERR_ULP_BASE = 0x1200,

    FIC_ERR_OTA_BASE = 0x1500,

    FIC_ERR_WIFI_BASE = 0x3000
};

#define FIC_ERR_RETURN_ON_FALSE(a, err_code, log_action) do {    \
        if (unlikely(!(a))) {                                   \
            log_action;                                         \
            return err_code;                                    \
        }                                                       \
    } while(0)

#define FIC_RETURN_VALUE_ON_ERROR(x, value, log_action) do { \
        fic_error_t err_rc_ = (x);                           \
        if (unlikely(err_rc_ != FIC_OK)) {                  \
            log_action;                                     \
            return value;                                   \
        }                                                   \
    } while(0)

#define FIC_RETURN_ON_ERROR(x, log_action) do {  \
        fic_error_t err_rc_ = (x);               \
        if (unlikely(err_rc_ != FIC_OK)) {       \
            log_action;                         \
            return err_rc_;                     \
        }                                       \
    } while(0)

#define FIC_RETURN_VOID_ON_ERROR(x, log_action) do {  \
        fic_error_t err_rc_ = (x);               \
        if (unlikely(err_rc_ != FIC_OK)) {       \
            log_action;                         \
            return;                             \
        }                                       \
    } while(0)

#endif