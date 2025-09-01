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

// **************************************************************************
//
// Modul-Name        : udpifcthread.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 17.02.2021
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ifcthread.h"
#include "udpifcthread.h"
#include "udpframeblk.h"



static void *ifcrunreader(void*parameter) {
    CUdpInterface* thread = (CUdpInterface*)(parameter);
    long           retval = 0;

    if (thread->InitInstance()) {
        retval = thread->RunRead();
    }

    return ((void*)retval);
}
static void *ifcrunwriter(void*parameter) {
    CUdpInterface* thread = (CUdpInterface*)(parameter);
    long           retval = 0;

    while (!(thread->mRunning));

    retval = thread->RunWrite();

    return ((void*)retval);
}

CUdpInterface::CUdpInterface(ModulId aModulId, const std::string &aModulName) : mLogger(aModulId, aModulName) {

}


bool CUdpInterface::InitInstance() {
    bool retval = true;

    return retval;
}


void CUdpInterface::Create(std::string aIfc, int aPort, std::string aName) {
    pthread_attr_t attr;
    /*
     * Store the interface spec.
     */
    mName = aName;
    mPort = aPort;
    mIfc  = aIfc;
    /*
     * Init the socket for reading and sending before we start the threads.
     */
    mHandle = socket(AF_INET, SOCK_DGRAM, 0) ;
    if (mHandle > -1) {
        /*
         * Set the listener to reuse address.
         */
        int optval = 1;
        setsockopt(mHandle, SOL_SOCKET, SO_REUSEADDR, (void *) &optval, sizeof(int));
        /*
         * Setup the address structure for the interface binding of the listen socket.
         */
        int err;
        struct sockaddr_in addr;
        socklen_t addrlen = sizeof(sockaddr);

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;

        addr.sin_addr.s_addr = inet_addr(mIfc.c_str());
        addr.sin_port = htons(aPort);
        while ((err = bind(mHandle, (struct sockaddr *) &addr, addrlen)) < 0) {
            if ((errno == EINVAL) || (errno == EADDRINUSE)) {
                struct timeval to = {10, 0};
                mLogger.Write(LogLevel::Warn, "Interface already bound. Waiting !!! Error %s", strerror(errno));
                select(0, 0, 0, 0, &to);
            } else {
                mLogger.Write(LogLevel::Error,  "Got error on bind to address: %s", strerror(errno));
                break;
            }
        }

        addrlen = sizeof(addr);
        mRunning = true;
        /*
         * Start the thread now
         */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&mReadIfcId, &attr, ifcrunreader, this);
        pthread_create(&mWriteIfcId, &attr, ifcrunwriter, this);
        mLogger.Write(LogLevel::Unbreakable, "Started UDP interface threads");
    } else {
        mLogger.Write(LogLevel::Error, "Could not create socket so not starting the threads.");
    }
}
//
//  The reader takes incoming packets from the socket and does some pre-processing on them
//  before sending them to the game loop.
int CUdpInterface::RunRead() {
    tUdpFrame frame;

    for (;mRunning;) {
        ssize_t recvd = recvmsg(mHandle, &frame.mMsg, 0);
        static_cast<void>(recvd);
    }
    return 0;
}
//
//  The sender waits on the queue and the socket and sends as much packages out as possible.
int CUdpInterface::RunWrite() {
    tUdpFrame frame;
    std::ostringstream oss;

    memset(&frame.mAddress, 0, sizeof(frame.mAddress));
    frame.mAddress.sin_family = AF_INET;
    frame.mAddress.sin_addr.s_addr = inet_addr(mIfc.c_str());
    frame.mAddress.sin_port = htons(mPort);

    for (;mRunning;) {
        //
        // 10ms waiting.
        struct timeval to={0, 10000};
        select(0, 0, 0, 0, &to);
    }
    return 0;
}
