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
