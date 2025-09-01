// MIT License
//
// Copyright (c) 2016 Hans-Juergen Lange<hjl@simulated-universe.de>
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


#pragma once
#ifndef TMSGENDCYCLEREQ_INC
#define TMSGENDCYCLEREQ_INC

#define IDM_ENDCYCLEREQ (0x516f21340e54ede9)

struct tMsgEndCycleReq : public tMsg {
    tMsgEndCycleReq() {id = IDM_ENDCYCLEREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    tMsgEndCycleReq(tJSON* j)  {(void)j; id = IDM_ENDCYCLEREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    virtual ~tMsgEndCycleReq() {}
    uint64_t    Cycle;
};

using tMsgEndCycleReqPtr = std::shared_ptr<tMsgEndCycleReq>;

#endif  // TMSGENDCYCLEREQ_INC
