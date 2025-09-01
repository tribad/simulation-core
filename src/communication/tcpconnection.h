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

#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <string>
#include <list>
#include <map>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "msg.h"

#include "connection.h"

class CTcpConnection : public CConnection
{
public:
    CTcpConnection(CLogger& aLogger);
    CTcpConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger, CProtocol* aProto=0);
    virtual ~CTcpConnection();

    virtual int Read(void);
    virtual int Write(std::shared_ptr<tNetPack> aPacket = std::shared_ptr<tNetPack>());

    virtual int Process(std::shared_ptr<tNetPack> aPacket);
    virtual int Process(std::shared_ptr<tMsg> aMsg);
    void Shutdown(void);
protected:
    /*
     * Personality info
     */
    uint32_t             Address;
    int                  Port;
private:
    /*
     * Outgoing attributes.
     */
    size_t               AlreadySend = 0U;
};

#endif // CCONNECTION_H
