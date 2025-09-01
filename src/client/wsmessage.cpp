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
//  Modul-Name     : wsmessage.cpp
//
//  Copyrights by Hans-Juergen Lange <hjl@simulated-universe.de>. All rights reserved.
//
// *************************************************************************************************************
#include <stdint.h>
#include <cstring>
#include <stdint.h>
#include <endian.h>
#include "wsmessage.h"
// Optional

namespace client {


    void tWSMessage::Reset(void) {
// User-Defined-Code:AAAAAAFyZJE0DSjx1YU=
        if (mPayload != nullptr) {
            delete[] mPayload;
        }
        //
        //  This only works as expected if all
        //  members are initialized with zero anyways.
        //  So if some member initialization other the zero exists
        //  this will probably fail
        memset(this, 0, sizeof(tWSMessage));
// End-Of-UDC:AAAAAAFyZJE0DSjx1YU=
    }

    bool tWSMessage::isComplete(void) {
        bool retval = false;
// User-Defined-Code:AAAAAAFyZJIBzSppzEo=
        retval = (mRecv >= mPayloadLen) && (mState >= 8);
// End-Of-UDC:AAAAAAFyZJIBzSppzEo=
        return (retval);
    }

    int64_t tWSMessage::Process(char *aData, int64_t aLength) {
        int64_t retval = 0u;
// User-Defined-Code:AAAAAAFyZJJy1ithvPc=
        char *s = aData;

        do {
            switch (mState) {
                case 0:
                    mFin = ((*s) & 0x80) == 0x80;
                    mRsv1 = ((*s) & 0x40) == 0x40;
                    mRsv2 = ((*s) & 0x20) == 0x20;
                    mRsv3 = ((*s) & 0x10) == 0x10;
                    mOpCode = ((*s) & 0x0f);
                    mHdrByteCount++;
                    mState = 1u;
                    break;
                case 1:
                    mMask = (((*s) & 0x80) == 0x80);
                    //
                    //  This is the client side. Servers do not mask their data.
                    //  So we do not unmask the data.
                    mPayloadLen = ((*s) & 0x7f);
                    switch (mPayloadLen) {
                        case 126:
                            mState = 2u;
                            break;
                        case 127:
                            mState = 3u;
                            break;
                        default:
                            if (mMask) {
                                mState = 4u;
                            } else {
                                mState = 8u;
                                if (mPayloadLen == 0) {
                                    mDone = true;
                                }
                            }
                            break;
                    }
                    mHdrByteCount++;
                    break;
                case 2:  //  ReadIn 16 Bit payload len.
                    if (mHdrByteCount == 2) {
                        mPayloadLen = (uint64_t) ((*s)) << 8;
                    } else {
                        mPayloadLen |= ((uint64_t) ((*s)) & 0x00000000000000ff);
                        if (mMask) {
                            mState = 4u;
                        } else {
                            mState = 8u;
                        }
                    }
                    mHdrByteCount++;
                    break;
                case 3:  //  ReadIn 64 Bit Payload len
                    switch (mHdrByteCount) {
                        case 2:
                            mPayloadLen = (uint64_t) ((*s)) << 56;
                            break;
                        case 3:
                            mPayloadLen |= (uint64_t) (*s) << 48;
                            break;
                        case 4:
                            mPayloadLen |= (uint64_t) ((*s)) << 40;
                            break;
                        case 5:
                            mPayloadLen |= (uint64_t) ((*s)) << 32;
                            break;
                        case 6:
                            mPayloadLen |= (uint64_t) ((*s)) << 24;
                            break;
                        case 7:
                            mPayloadLen |= (uint64_t) ((*s)) << 16;
                            break;
                        case 8:
                            mPayloadLen |= (uint64_t) ((*s)) << 8;
                            break;
                        case 9:
                            mPayloadLen |= (uint64_t) ((*s));
                            if (mMask) {
                                mState = 4u;
                            } else {
                                mState = 8u;
                            }
                            break;
                        default:
                            break;
                    }
                    mHdrByteCount++;
                    break;
                case 4: // Mask byte 1
                    mState = 5u;
                    mMaskKey[0] = (*s);
                    mHdrByteCount++;
                    break;
                case 5: // Mask byte 2
                    mState = 6;
                    mMaskKey[1] = (*s);
                    mHdrByteCount++;
                    break;
                case 6: // Mask byte 3
                    mState = 7;
                    mMaskKey[2] = (*s);
                    mHdrByteCount++;
                    break;
                case 7: // Mask byte 4
                    mState = 8;
                    mMaskKey[3] = (*s);
                    mHdrByteCount++;
                    if (mPayloadLen == 0) {
                        mDone = true;
                    }
                    break;
                case 8:   //  first byte. allocate buffer.
                    if (mPayloadLen > 0) {
                        //
                        mState = 9;
                        /*
                         * restart recv counter
                         */
                        mRecv = 0u;
                        /*
                         * Create the payload buffer.
                         * We must copy the data to get them encoded.
                         */
                        mPayload = new char[mPayloadLen];
                    } else {
                        mDone = true;
                        break;
                    }
                    //
                    // Do not break out we are doing the same as in default. S
                    // So do not move this state into another place nor add more states here.
                    [[fallthrough]];
                default:
                    if (mMask) {
                        mPayload[mRecv] = (*s) ^ mMaskKey[mRecv % 4];
                    } else {
                        mPayload[mRecv] = (*s);
                    }
                    mRecv++;
                    if (mRecv == mPayloadLen) {
                        mDone = true;
                    }
                    break;
            }
            s++;
        } while (((s - aData) < (aLength)) && ((mPayloadLen == 0) || (mRecv < mPayloadLen)));

        retval = s - aData;
// End-Of-UDC:AAAAAAFyZJJy1ithvPc=
        return (retval);
    }

    char* tWSMessage::pack(char *a_buffer) {
        char *s = a_buffer;
        //
        //  Set the bits
        *s = (mFin << 7) | (mRsv1 << 6) | (mRsv2 << 5) | (mRsv3 << 4) | mOpCode;
        s++;
        //
        //  Set the payload length.
        if (mPayloadLen > 125u) {
            if (mPayloadLen > 65535u) {
                *s = 127u | (mMask << 7);

                uint64_t swapped = htobe64(mPayloadLen);

                s++;
                memcpy(s, &swapped, sizeof(swapped));
                s += sizeof(swapped);
            } else {
                *s = 126u | (mMask << 7);
                s++;
                *s = mPayloadLen >> 8u;
                s++;
                *s = mPayloadLen & 0x00000000000000ffu;
                s++;
            }
        } else {
            *s  = (mMask << 7) | (mPayloadLen & 0x7f);
            s++;
        }
        //
        //  If we have a mask set the mask.
        if (mMask) {
            memcpy(s, mMaskKey, sizeof(mMaskKey));
            s+=sizeof(mMaskKey);
        }
        //
        //  The masking operation is the same as on the receiver side.
        for (size_t i = 0; i < mPayloadLen; ++i) {
            if (mMask) {
                (*s) = mPayload[i] ^ mMaskKey[i % 4];
            } else {
                (*s) = mPayload[i];
            }
            s++;
        }
        return s;
    }
};

