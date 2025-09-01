#include "commtarget.h"
#include "objectref.h"

#if 0
tCommTarget tCommTarget::mainobj = {0,0};
tCommTarget::tCommTarget(const tObjectRef& target) {type = eCommTarget::Object; obj.m_id = target; obj.m_ptr = target;}
tCommTarget::tCommTarget(uint64_t aId, tSimObj* aPtr) {type = eCommTarget::Object; obj.m_id = aId; obj.m_ptr = aPtr;}
#endif
