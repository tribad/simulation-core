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

#include "logger.h"
#include "commthread.h"
#include "httpsimulationifc.h"
#include <httpdomainspec.h>

CHttpSimulationIfc::CHttpSimulationIfc(CLogger& aLog, CHttpDomainSpec* aParent) : Log(aLog)
{
    Parent = aParent;
}

void CHttpSimulationIfc::SendMsg(uint64_t a_connid, tMsg *msg) {
    CCommThread::instance().Put(a_connid, msg);
}

void CHttpSimulationIfc::SendSig(uint64_t a_connid, tSig *sig) {
    CCommThread::instance().Put(a_connid, sig);
}

void CHttpSimulationIfc::SendCoreMsg(uint64_t a_connid, tMsg *msg) {
    CCommThread::instance().Put(a_connid, msg);
}

void CHttpSimulationIfc::SendCoreSig(uint64_t a_connid, tSig *sig) {
    CCommThread::instance().Put(a_connid, sig);
}

std::shared_ptr<CCacheEntry> CHttpSimulationIfc::GetHtml(const std::string& aFileName) {
    std::shared_ptr<CCacheEntry> retval;

    if (Parent != nullptr) {
        std::string html  = Parent->HtmlDir;
        std::string aPath = html + "/" + aFileName;

        retval = CCacheThread::instance().Get(aPath);
    }

    return retval;
}
std::shared_ptr<CCacheEntry> CHttpSimulationIfc::GetContent(const std::string& aFileName) {
    std::shared_ptr<CCacheEntry> retval;

    if (Parent != nullptr) {
        std::string content  = Parent->ContentDir;
        std::string aPath = content + "/" + aFileName;

        retval = CCacheThread::instance().Get(aPath);
    }
    return retval;
}
std::shared_ptr<CCacheEntry> CHttpSimulationIfc::GetData(const std::string& aFileName) {
    std::shared_ptr<CCacheEntry> retval;

    if (Parent != nullptr) {
        std::string data  = Parent->DataDir;
        std::string aPath = data + "/" + aFileName;

        retval = CCacheThread::instance().Get(aPath);
    }
    return retval;
}
