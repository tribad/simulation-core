// MIT License
//
// Copyright (c) 2024 Hans-Juergen Lange<hjl@simulated-universe.de>
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

// *************************************************************************************************************
//
//  Modul-Name     : wsmessage.h
//
//  Copyrights by Hans-Juergen Lange <hjl@simulated-universe.de>. All rights reserved.
//
// *************************************************************************************************************
#pragma once
#ifndef WSMESSAGE_INC
#define WSMESSAGE_INC

#include <cstdint>

namespace client {

//
//  This is the class
class tWSMessage {
public:
    tWSMessage() = default;
    tWSMessage(char* aData, int64_t aLength);
    ~tWSMessage() = default;
    void Reset(void) ;
    bool isComplete(void) ;
    int64_t Process(char* aData, int64_t aLength) ;
    char* pack(char* a_buffer);
public:
    bool     mDone = false;
    uint8_t  mState = 0u;
    uint8_t  mHdrByteCount = 0u;
    uint64_t mRecv = 0u;
    bool     mFin = false;
    bool     mRsv1 = false;
    bool     mRsv2 = false;
    bool     mRsv3 = false;
    uint8_t  mOpCode = 0u;
    bool     mMask = false;
    uint64_t mPayloadLen = 0u;
    uint8_t  mMaskKey[4] = {0u};
    char*    mPayload = nullptr;
};

}

#endif  // WSMESSAGE_INC
