// MIT License
//
// Copyright (c) 2018 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#include <set>
#include "httpdomainspec.h"
#include "httpsconnection.h"
#include "logger.h"
#include "ifcthread.h"
#include "sslifcthread.h"
#include "httpsifcthread.h"

CHTTPSIfcThread::CHTTPSIfcThread() : CSSLIfcThread(ModulId::HttpSSLServer, "HTTPS-Ifc")
{
    std::set<CHttpDomainSpec*> specs;
    //
    //  Lock the domain map access
    CHTTPProtocol::domains_lock.lock();
    for (auto d : CHTTPProtocol::domains) {
        if (specs.find(d.second) == specs.end()) {
            //d.second->InitNotifier(aWatchList);
        }
    }
    CHTTPProtocol::domains_lock.unlock();

}

std::shared_ptr<CConnection> CHTTPSIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CHTTPSConnection>(aHandle, aConnectionId, ifcLogger));
}
