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

#ifndef CSSLIFCTHREAD_H
#define CSSLIFCTHREAD_H

#include <gnutls/gnutls.h>
#include <string>
#include <list>
#include <mutex>
#include "sslconnection.h"

class CSSLIfcThread : public CIfcThread
{
public:
    CSSLIfcThread(ModulId aModulId, const std::string& aModulName );
    void Create(std::string aIfc, int aPort, const std::string& aCertPath,
                const std::string& aKeyPath, const std::string& aCAPath,
                const int aUpdateCert=1);
    virtual bool InitInstance();
    virtual void ExtendedSetup(std::shared_ptr<CConnection> aConnection);
private:
    virtual void OnInterfaceTimer(uint64_t aTimerId);
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
private:
    int                    mUpdateCertTime;
    std::shared_ptr<CX509> mX509;
    std::string            mCAPath;
    std::string            mCertPath;
    std::string            mKeyPath;
};

#endif // CSSLIFCTHREAD_H
