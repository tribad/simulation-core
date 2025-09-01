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

#include "coreifc.h"
#include "sim.h"

/*
 * This is the interface into the simulation core
 */
CCoreIfc*    __coreIfc;

CCoreIfc::CCoreIfc()
{

}

CCoreIfc::~CCoreIfc() {

}

void CCoreIfc::StartObject(const tObjectRef& obj) {
    simaddobject(obj);
}

void CCoreIfc::FreeObject(const tObjectRef& obj) {
    simdelobject(obj);
}

void CCoreIfc::ScheduleMsg(std::shared_ptr<tMsg> aMsg, uint64_t aDelay) {
    //
    //
    (void)aDelay;
    //
    simaddmsg(aMsg);
}

void CCoreIfc::ScheduleSignal(std::shared_ptr<tSig> aSig, uint64_t aDelay) {
    //
    //
    (void)aDelay;
    //
    simaddmsg(aSig);
}
