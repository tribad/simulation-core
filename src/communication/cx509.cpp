// MIT License
//
// Copyright (c) 2021 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
