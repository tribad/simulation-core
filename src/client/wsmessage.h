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
