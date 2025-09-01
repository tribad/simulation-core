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

#ifndef HTTPSIMULATIONIFC_H
#define HTTPSIMULATIONIFC_H

#include <cstdint>
#include <msg.h>
#include <logger.h>
#include <cachethread.h>

class tSQLRecordSet;
class CHttpDomainSpec;
class IFileCacheNotifier;

class CHttpSimulationIfc
{
public:
    CHttpSimulationIfc(CLogger& aLog, CHttpDomainSpec* aParent);
    //
    //  DB-Access
    int DBConnect(const char* dbname, const char *user, const char *password, const char* port, const char* host);
    tSQLRecordSet* DBExec(int handle, const char* cmd);
    void DBClose(int handle);
    //
    //  In-Core messaging
    void SendMsg(uint64_t a_connid, tMsg* msg);
    void SendSig(uint64_t a_connid, tSig* sig);
    void SendCoreMsg(uint64_t a_connid, tMsg* msg);
    void SendCoreSig(uint64_t a_connid, tSig* sig);
    //
    //  File Cache
    std::shared_ptr<CCacheEntry> GetHtml(const std::string& aFileName);
    std::shared_ptr<CCacheEntry> GetContent(const std::string& aFileName);
    std::shared_ptr<CCacheEntry> GetData(const std::string& aFileName);
    //
    //  Logging
    CLogger& Log;
private:
    CHttpDomainSpec* Parent;
};

#endif // HTTPSIMULATIONIFC_H
