// MIT License
//
// Copyright (c) 2020 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#include "simobj.h"

void simaddobject(const tObjectRef& newobject);

tSimObj::tSimObj() {

}

tSimObj::~tSimObj() {

}

void tSimObj::Start() {
    void simaddobject(const tObjectRef& newobject);
}

void tSimObj::SetValue(valueid_t vid, valueindex_t vidx, const tVariant& value) {
    (void)vid;
    (void)vidx;
    (void)value;
}

tVariant tSimObj::GetValue(valueid_t vid, valueindex_t vidx){
    (void)vid;
    (void)vidx;
    tVariant retval;

    return retval;
}

void tSimObj::SetReference(uint64_t vid, uint64_t vidx, const tObjectRef& value){
    (void)vid;
    (void)vidx;
    (void)value;
}

tObjectRef tSimObj::GetReference(uint64_t vid, uint64_t vidx){
    tObjectRef retval;

    (void)vid;
    (void)vidx;

    return retval;
}

void tSimObj::RemoveReference(uint64_t vid, uint64_t vidx) {
    (void) vid;
    (void) vidx;
}

void tSimObj::InitMember(valueid_t vid, valueindex_t vidx, const tVariant& value){
    (void)vid;
    (void)vidx;
    (void)value;
}

//
//  This is the virtual implementation as we do want allow compilation without
//  using shared ptrs.
tMsg* tSimObj::Process(std::shared_ptr<tMsg> msg) {
    (void)msg;

    return nullptr;
}

void tSimObj::SetParent(const tObjectRef &obj)  {
    if ((parenttype == 0ul) || (!obj.valid()) || (obj->type == parenttype)) {
        parent = obj;
    }
}

bool tSimObj::update(uint64_t aCycle) {
    static_cast<void>(aCycle);
    return false;
}

