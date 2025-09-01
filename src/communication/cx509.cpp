//
// Created by ltoo on 04.12.21.
//
#include <gnutls/gnutls.h>

#include "cx509.h"

CX509::CX509(const std::string &aCertPath, const std::string &aKeyPath, const std::string &aCAPath) {
    int  gtlserror = GNUTLS_E_SUCCESS;
    /*
     * Silence compiler warning
     */
    static_cast<void>(aCAPath);
    /*
     * Prevent the usage of the certs while loading them.
     */
    if (gnutls_certificate_allocate_credentials(&credentials) == GNUTLS_E_SUCCESS) {
        gtlserror = gnutls_certificate_set_x509_key_file(credentials, aCertPath.c_str(), aKeyPath.c_str(), GNUTLS_X509_FMT_PEM);
        if (gtlserror == GNUTLS_E_SUCCESS) {
            gtlserror = gnutls_priority_init(&priority_cache, NULL, NULL);
#if GNUTLS_VERSION_NUMBER >= 0x030506
            gtlserror = gnutls_certificate_set_known_dh_params(credentials, GNUTLS_SEC_PARAM_MEDIUM);
#else
            gnutls_dh_params_t dh_params;
            gtlserror = gnutls_dh_params_init(&dh_params);
            //gtlserror = gnutls_dh_params_import_dsa(dh_params, x509_cred);

            gnutls_certificate_set_dh_params(x509_cred, dh_params);
#endif
        }
    }
}

CX509::~CX509() {
    //
    //  Free some memory
    gnutls_certificate_free_credentials(credentials);
    gnutls_priority_deinit(priority_cache);
}
