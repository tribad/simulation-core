// MIT License
//
// Copyright (c) 2017 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "netpack.h"
#include "simifc.h"
#include "timer.h"
#include "logger.h"
#include "ifcthread.h"
#include "sslifcthread.h"
#include "sslconnection.h"

#define CERT_UPDATE_TIMER 0x55aabbcc

CSSLIfcThread::CSSLIfcThread(ModulId aModulId, const std::string& aModulName ) : CIfcThread(aModulId, aModulName)
{

}

void CSSLIfcThread::Create(std::string aIfc, int aPort, const std::string& aCertPath,
                           const std::string& aKeyPath, const std::string& aCAPath,
                           const int aUpdateCert) {
    mCAPath         = aCAPath;
    mCertPath       = aCertPath;
    mKeyPath        = aKeyPath;
    mUpdateCertTime = aUpdateCert;
    CIfcThread::Create(aIfc, aPort);
}

bool CSSLIfcThread::InitInstance() {
    bool retval     = false;
    int  loadresult = GNUTLS_E_SUCCESS;

    mX509 = std::make_shared<CX509>(mCertPath, mKeyPath, mCAPath);
    if (loadresult == GNUTLS_E_SUCCESS) {
        retval = CIfcThread::InitInstance();
        if (retval) {
            addtimer(mUpdateCertTime*60ul*1000ul, 0, SignalNo, CERT_UPDATE_TIMER);
        }
    }
    return retval;
}

void CSSLIfcThread::ExtendedSetup(std::shared_ptr<CConnection> aConnection) {

    if (aConnection != nullptr) {
        /*
         * Do all the tls stuff before going on with the connection.
         */
        std::shared_ptr<CSSLConnection> sslconn = std::static_pointer_cast<CSSLConnection>(aConnection);

        sslconn->Setup(mX509);
    } else {
    }
}

std::shared_ptr<CConnection> CSSLIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CSSLConnection>(aHandle, aConnectionId, ifcLogger));
}


void CSSLIfcThread::OnInterfaceTimer(uint64_t aTimerId) {
    switch (aTimerId) {
    case CERT_UPDATE_TIMER:
        mX509 = std::make_shared<CX509>(mCertPath, mKeyPath, mCAPath);
        addtimer(mUpdateCertTime*60ul*1000ul, 0, SignalNo, CERT_UPDATE_TIMER);
        break;
    default:
        CIfcThread::OnInterfaceTimer(aTimerId);
        break;
    }
}
