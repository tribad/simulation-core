//
// Created by ltoo on 04.12.21.
//

#ifndef APPCORE_CX509_H
#define APPCORE_CX509_H

#include <string>
#include <gnutls/gnutls.h>


class CX509 {
public:
    CX509() = default;
    CX509(const std::string& aCertPath,
           const std::string& aKeyPath, const std::string& aCAPath);
    ~CX509();
    CX509& operator=(const CX509 &) = default;
public:
    gnutls_priority_t                priority_cache = nullptr;
    gnutls_certificate_credentials_t credentials    = nullptr;
};


#endif //APPCORE_CX509_H
