#ifndef SNTP_PROVIDER_HAL_HH
#define SNTP_PROVIDER_HAL_HH

#include <string>

#include "fic_errors.hh"

class ISNTPLifetime {
public:
    virtual ~ISNTPLifetime() = default;

    virtual fic_error_t init() = 0;
    virtual fic_error_t deinit() = 0;
};

class ISNTPClient {
public:
    virtual ~ISNTPClient() = default;

    virtual fic_error_t obtain_time() = 0;
    virtual fic_error_t set_server(std::string_view server_name) = 0;
};


#endif