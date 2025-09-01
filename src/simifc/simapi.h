#ifndef SIMAPI_H
#define SIMAPI_H

#include <simobj.h>
#include <membervaluearray.h>

#define STDB_SQL_EXECLIMIT 10000
//
// CoreIfc access functions.
void ScheduleSignal(tSig *sig);
void ScheduleMsg(tMsg* msg);
void StartObject(const tObjectRef& obj);
void FreeObject(const tObjectRef& obj);
//
//  This function removes an object from the simulation.
//  Any processing needed for clean up is done here.
//  Final removal is done in the simulation thread.
void     DeleteObject(const tObjectRef& obj);
//
//  Setting the absolut cycle of the simulation.
//  Only the absolut-cycle can be set to a specific value.
void SetCycle(uint64_t aCycle);
uint64_t GetCycle(void);

tMsg* SendMsg(const tObjectRef& aRef, std::shared_ptr<tMsg> aMsg);
tMsg* SendMsg(tSimObj* aRef, std::shared_ptr<tMsg> aMsg);
tMsg* SendMsg(std::shared_ptr<tMsg> aMsg);

void SendSig(const tObjectRef& aRef, std::shared_ptr<tSig> aSig);
void SendSig(tSimObj* aRef, std::shared_ptr<tSig> aSig);
void SendSig(std::shared_ptr<tSig> aSig);

#ifdef FAST_MESSAGE_PROCESSING
static inline tMsg* SendRequest(std::shared_ptr<tMsg> aMsg) {
    if (aMsg != nullptr) {
        aMsg->type |= MSG_TYPE_REQUEST;
        return SendMsg(aMsg);
        }
    }
    return nullptr;
}
static inline void SendReply(std::shared_ptr<tMsg> aMsg) {
    if (aMsg != nullptr) {
        aMsg->type |= MSG_TYPE_REPLY;
        tMsg* answer = SendMsg(aMsg);
        if (answer) {
            delete answer;
        }
    }
}
static inline void SendIndication(std::shared_ptr<tMsg> aSignal) {
    if (aSignal != nullptr) {
        aSignal->type |= MSG_TYPE_INDICATION;
        SendSig(aSignal);
    }
}

static inline void SendConfirmation(std::shared_ptr<tMsg> aSignal) {
    if (aSignal != nullptr) {
        aSignal->type |= MSG_TYPE_CONFIRM;
        SendSig(aSignal);
    }
}

static inline tMsg* SendMsgToParent(const tObjectRef& aRef,std::shared_ptr<tMsg> aMsg) {
    tMsg*      retval = nullptr;
    tObjectRef parent = aRef->parent;

    if (parent.valid()) {
        retval = parent->Process(aMsg);
    }
    return retval;
}

static inline tMsg* SendMsgToParent(const tSimObj* aObj, std::shared_ptr<tMsg> aMsg) {
    tMsg*      retval = nullptr;

    if (aObj != 0) {
        tObjectRef parent = aObj->parent;

        if (parent.valid()) {
            retval = parent->Process(aMsg);
        }
    }
    return retval;
}
#else
tMsg* SendRequest(std::shared_ptr<tMsg> aMsg);
void SendReply(std::shared_ptr<tMsg> aMsg) ;
void SendIndication(std::shared_ptr<tMsg> aSignal) ;
void SendConfirmation(std::shared_ptr<tMsg> aSignal) ;
tMsg* SendMsgToParent(const tObjectRef& aRef,std::shared_ptr<tMsg> aMsg) ;
tMsg* SendMsgToParent(const tSimObj* aObj, std::shared_ptr<tMsg> aMsg) ;
#endif
void SendSigToParent(const tObjectRef& aRef,std::shared_ptr<tSig> aSig);
void SendSigToParent(const tSimObj* aObj, std::shared_ptr<tSig> aMsg);

tObjectRef GetParent(const tObjectRef& aRef);

void SetValue(const tObjectRef& ref, uint64_t vid, uint64_t vidx, const tVariant& value);
void SetValue(const tObjectRef& ref, uint64_t vid, const tVariant& value);

tVariant GetValue(tSimObj* obj, uint64_t vid, uint64_t vidx);
tVariant GetValue(tSimObj* obj, uint64_t vid);
void SetValue(tSimObj* obj, uint64_t vid, uint64_t vidx, const tVariant& value);
void SetValue(tSimObj* obj, uint64_t vid, const tVariant& value);
//
//  Wrappers for direct access to the db.
//  All these functions work asynchronous.
//  We use different Namespaces to destinguish between the different DB-Types. Its up to the application to make the decission.
namespace mariadb {
    extern void open(tCommTarget sender, std::string name, std::string host="", std::string port="", std::string user="", std::string passwd="");
    extern void sqlexecstring(tCommTarget sender, int handle, uint64_t cmdtype, const char* cmd, uint32_t limit = (uint32_t)-1 );
    extern void sqlfetch(tCommTarget sender, int handle, uint32_t limit = (uint32_t)-1);
    extern void close(tCommTarget sender, int handle);
}

namespace pgdb {
    extern void open(tCommTarget sender, std::string name, std::string host="", std::string port="", std::string user="", std::string passwd="");
    extern void sqlexecstring(tCommTarget sender, int handle, uint64_t cmdtype, const char* cmd, uint32_t limit = (uint32_t)-1 );
    extern void sqlfetch(tCommTarget sender, int handle, uint32_t limit = (uint32_t)-1);
    extern void close(tCommTarget sender, int handle);
}

namespace stdb {
    extern void open(tCommTarget sender, std::string name, std::string host="", std::string port="", std::string user="", std::string passwd="");
    extern void sqlexecstring(tCommTarget sender, int handle, uint64_t cmdtype, const char* cmd, uint32_t limit = (uint32_t)-1 );
    extern void sqlfetch(tCommTarget sender, int handle, uint32_t limit = (uint32_t)-1);
    extern void close(tCommTarget sender, int handle);
}
/*
 * These are more sophisticated wrappers to the CoreIfc Structure
 */
extern void stdb_createobj(objectid_t objid, uint64_t type, uint64_t droptime) ;
extern void stdb_createfromtemplate(uint32_t templateid, uint32_t objid, uint64_t type) ;
extern void stdb_deleteobj(objectid_t objid) ;

extern tSimObj* stdb_loadobj(objectid_t objid) ;

extern void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, double value) ;
extern void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, double value) ;
extern void stdb_deletefloatdata(objectid_t objid, valueid_t valueid, valueindex_t index) ;
extern void stdb_set(objectid_t objid, valueid_t valueid, valueindex_t index, double value);
extern void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, uint64_t value) ;
extern void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, uint64_t value) ;
extern void stdb_set(objectid_t objid, valueid_t valueid, valueindex_t index, uint64_t value);
extern void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, bool value) ;
extern void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, bool value) ;
extern void stdb_set(objectid_t objid, valueid_t valueid, valueindex_t index, bool value);
extern void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, int64_t value) ;
extern void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, int64_t value) ;
extern void stdb_deleteintdata(objectid_t objid, valueid_t valueid, valueindex_t index) ;
extern void stdb_set(objectid_t objid, valueid_t valueid, valueindex_t index, int64_t value);
extern void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, const char*value) ;
extern void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, const char*value) ;
extern void stdb_deletetextdata(objectid_t objid, valueid_t valueid, valueindex_t index) ;
extern void stdb_set(objectid_t objid, valueid_t valueid, valueindex_t index, const char* value);
extern void stdb_createreferencedata(objectid_t objid, valueid_t valueid, valueindex_t index, objectid_t reference) ;
extern void stdb_updatereferencedata(objectid_t objid, valueid_t valueid, valueindex_t index, objectid_t reference) ;
extern void stdb_deletereferencedata(objectid_t objid, valueid_t valueid, valueindex_t index) ;
extern void stdb_sqlexecstring(tCommTarget sender, uint64_t cmdtype, const char* cmd, uint64_t limit = STDB_SQL_EXECLIMIT);
extern void stdb_fetch(tCommTarget sender, uint64_t cmdtype, uint64_t limit = STDB_SQL_EXECLIMIT);
/*
 * JSON helper.
 * As the JSON parser works on standard data types and is used by different applications the
 * extensions needed for the simulation are defined here.
 */
objectid_t to_objectid_t(tJSON* j);


#endif // SIMAPI_H
