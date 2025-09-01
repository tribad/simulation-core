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

