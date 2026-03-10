#ifndef CREDENTIALS_PROVIDER_HH
#define CREDENTIALS_PROVIDER_HH

#include <string>

#include "fic_errors.hh"

class ICredentialsProvider {
public:
    virtual ~ICredentialsProvider() = default;

    virtual std::string_view get_client_cert() const = 0;
    virtual std::string_view get_client_key() const = 0;
    virtual std::string_view get_ca_cert() const = 0;
};

#endif