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

#ifndef HTTPCONTENTIFC_H
#define HTTPCONTENTIFC_H

#include <cstdint>
#include <helper.h>
#include <msg.h>
#include <logger.h>

class tHttpResponse;
class tHttpRequest;
class CHttpSimulationIfc;
class CFileCacheEntry;
class IFileCacheNotifier;

class CHttpContentIfc
{
public:
    CHttpContentIfc();
    virtual ~CHttpContentIfc();
    virtual bool DoYouHandleURI(const char* uri);
    virtual tHttpResponse* HandleURI(tHttpRequest* req);
    virtual tHttpResponse* Process(tHttpRequest* req, std::shared_ptr<tMsg> msg);
    bool ReadyToRelease() {return (RequestCount == ResponseCount);}
protected:
    uint64_t RequestCount;
    uint64_t ResponseCount;
};

typedef CHttpContentIfc* (*httpcontentifc_op)(xmlNode*, CHttpSimulationIfc*);

#endif // HTTPCONTENTIFC_H
