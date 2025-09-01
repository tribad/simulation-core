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

// *************************************************************************************************************
//
//  Modul-Name     : tMsgWSDisconnectReq.h
//
//  Copyrights by Hans-Juergen Lange <hjl@simulated-universe.de>. All rights reserved.
//
// *************************************************************************************************************
// HOC *********************************************************************************************************
//          |         |
// YYYYMMDD | Creator | description
//--------------------------------------------------------------------------------------------------------------
//          |         |
//----------+---------+-----------------------------------------------------------------------------------------
//          |         |
// HOC end *****************************************************************************************************
#pragma once
#ifndef TSIGWSDISCONNECTINDICATION_H
#define TSIGWSDISCONNECTINDICATION_H

#define IDS_TSIGWSDISCONNECTINDICATION (0xf9c819c3c6a2701b)

struct tSigWSDisconnectIndication : public tMsg {
    tSigWSDisconnectIndication() {id = IDS_TSIGWSDISCONNECTINDICATION;type = MSG_TYPE_INDICATION|MSG_TYPE_INTERNAL;}
    tSigWSDisconnectIndication(tJSON* j)  {(void)j; id = IDS_TSIGWSDISCONNECTINDICATION;type = MSG_TYPE_INDICATION|MSG_TYPE_INTERNAL;}
    virtual ~tSigWSDisconnectIndication() {}
};

using tSigWSDisconnectIndicationPtr = std::shared_ptr<tSigWSDisconnectIndication>;

#endif // TSIGWSDISCONNECTINDICATION_H
