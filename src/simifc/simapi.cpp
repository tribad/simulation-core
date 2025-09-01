// MIT License
//
// Copyright (c) 2019 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#include <iostream>
#include <iomanip>
#include <simapi.h>
#include <dbmsg.h>
#include <coreifc.h>
#include <tSigSQLExecStringReq.h>
#include <tSigSQLConnectReq.h>
#include <tSigSQLDisconnectReq.h>
#include <tSigSQLFetchReq.h>
#include <tSigObjectLoadReq.h>

#include <msgqueue.h>
#include <sharedmsgqueue.h>

extern CCoreIfc* __coreIfc;

void ScheduleSignal(tSig *sig) {
    //__coreIfc->ScheduleSignal(sig);
    std::cerr << "Shall schedule signal: 0x" << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') <<  sig->id << std::endl;
}

void ScheduleMsg(tMsg *msg) {
    //__coreIfc->ScheduleMsg(msg);
    std::cerr << "Shall schedule message: 0x" << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') <<  msg->id << std::endl;
}

void StartObject(const tObjectRef& obj) {
    __coreIfc->StartObject(obj);
}

void FreeObject(const tObjectRef& obj) {
    __coreIfc->FreeObject(obj);
}

void SetCycle(uint64_t aCycle) {
    __coreIfc->absCycle = aCycle;
}

uint64_t GetCycle() {
    return (__coreIfc->absCycle);
}


void DeleteObject(const tObjectRef& obj) {
    //
    //  In between we must free the index as well.
    stdb_deleteobj(obj->objid);
}


tMsg* SendMsg(const tObjectRef &ref, std::shared_ptr<tMsg> msg) {
    tMsg*      retval = nullptr;

    if (ref.valid()) {
        retval = ref->Process(msg);
    }
    return retval;
}

tMsg* SendMsg(tSimObj* ptr, std::shared_ptr<tMsg> msg) {
    tMsg*      retval = nullptr;

    if (ptr != nullptr) {
        retval = ptr->Process(msg);
    }
    return retval;
}

tMsg* SendMsg(std::shared_ptr<tMsg> msg) {
    tMsg* retval = nullptr;

    if (std:: holds_alternative<tConnection>(msg->dst)) {
        __coreIfc->comm->Put(msg);
    } else if (std:: holds_alternative<tDBQueue>(msg->dst)) {
        __coreIfc->stdb->Put(msg);
    } else if (std:: holds_alternative<tReference>(msg->dst)) {
        auto ref = std::get<tReference>(msg->dst);
        tObjectRef obj(ref.m_id, (tSimObj *) ref.m_ptr);
        //
        //  This is needed to allow on-the-fly loading of objects from the DB.
        if (obj.Raw() == nullptr) {
            __coreIfc->comm->Put(msg);
        } else {
            retval = obj->Process(msg);
        }
    } else {
        __coreIfc->comm->Put(msg);
    }
    return retval;
}

void SendSig(const tObjectRef &ref, std::shared_ptr<tSig> sig) {
    tMsg* reply;

    if (ref.valid()) {
        reply = ref->Process(sig);
        if (reply != nullptr) {
            delete reply;
        }
    }
}


void SendSig(tSimObj* ptr, std::shared_ptr<tSig> sig) {
    tMsg*      retval;

    if (ptr != nullptr) {
        retval = ptr->Process(sig);
        if (retval != nullptr) {
            delete retval;
        }
    }
}

void SendSig(std::shared_ptr<tSig> sig) {
    if (std:: holds_alternative<tConnection>(sig->dst)) {
        __coreIfc->comm->Put(sig);
    } else if (std:: holds_alternative<tDBQueue>(sig->dst)) {
        __coreIfc->stdb->Put(sig);
    } else if (std:: holds_alternative<tReference>(sig->dst)) {
        auto ref = std::get<tReference>(sig->dst);
        tObjectRef obj(ref.m_id, (tSimObj *) ref.m_ptr);
        //
        //  This is needed to allow on-the-fly loading of objects from the DB.
        if (obj.Raw() == nullptr) {
            __coreIfc->comm->Put(sig);
        } else {
            auto retval = obj->Process(sig);
            //
            //  If we geht an answer this is silently dropped.
            if (retval) {
                delete retval;
            }
        }
    } else {
        __coreIfc->comm->Put(sig);
    }
}

#ifndef FAST_MESSAGE_PROCESSING

tMsg* SendRequest(std::shared_ptr<tMsg> aMsg) {
    if (aMsg != nullptr) {
        aMsg->type |= MSG_TYPE_REQUEST;
        return SendMsg(aMsg);
    }
    return nullptr;
}

void SendReply(std::shared_ptr<tMsg> aMsg) {
    if (aMsg != nullptr) {
        aMsg->type |= MSG_TYPE_REPLY;
        tMsg* answer = SendMsg(aMsg);
        if (answer) {
            delete answer;
        }
    }
}

void SendIndication(std::shared_ptr<tMsg> aSignal) {
    if (aSignal != nullptr) {
        aSignal->type |= MSG_TYPE_INDICATION;
        SendSig(aSignal);
    }
}

void SendConfirmation(std::shared_ptr<tMsg> aSignal) {
    if (aSignal != nullptr) {
        aSignal->type |= MSG_TYPE_CONFIRM;
        SendSig(aSignal);
    }
}

tMsg* SendMsgToParent(const tObjectRef &ref, std::shared_ptr<tMsg> msg) {
    tMsg*      retval = 0;
    tObjectRef parent = ref->parent;

    if (parent.valid()) {
        retval = parent->Process(msg);
    }
    return retval;
}

tMsg* SendMsgToParent(const tSimObj* obj, std::shared_ptr<tMsg> msg) {
    tMsg*      retval = 0;

    if (obj != 0) {
        tObjectRef parent = obj->parent;

        if (parent.valid()) {
            retval = parent->Process(msg);
        }
    }
    return retval;
}
#endif
void SendSigToParent(const tObjectRef &ref, std::shared_ptr<tSig> sig) {
    tObjectRef parent = ref->parent;

    if (parent.valid()) {
        tMsg* retval = parent->Process(sig);

        if (retval != nullptr) {
            delete retval;
        }
    }
}

void SendSigToParent(const tSimObj* obj, std::shared_ptr<tSig> sig) {
    if (obj != nullptr) {
        tObjectRef parent = obj->parent;

        if (parent.valid()) {
            tMsg* retval = parent->Process(sig);

            if (retval != nullptr) {
                delete retval;
            }
        }
    }
}

tObjectRef GetParent(const tObjectRef &ref) {
    tObjectRef   retval = 0;

    retval = ref->parent;

    return retval;
}


void stdb_createobj(objectid_t objid, typeid_t type, uint64_t droptime) {

    if (__coreIfc->stdb != 0) {
        tMsgDBCreateObjectPtr msg = newPtr (tMsgDBCreateObject);

        msg->id=eMsgDBCreateObject;
        msg->objectid = objid;
        msg->type     = type;
        msg->droptime = droptime;
        msg->handle   = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_createfromtemplate(templateid_t templateid, objectid_t objid, typeid_t type) {

    if (__coreIfc->stdb != 0) {
        tMsgDBCreateFromTemplatePtr msg = newPtr(tMsgDBCreateFromTemplate);

        msg->id=eMsgDBCreateFromTemplate;
        msg->templateid=templateid;
        msg->objectid=objid;
        msg->type=type;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_deleteobj(objectid_t objid) {

    if (__coreIfc->stdb != 0) {
        tMsgDBDeleteObjectPtr msg = newPtr (tMsgDBDeleteObject);

        msg->id=eMsgDBDeleteObject;
        msg->objectid=objid;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

tSimObj* stdb_loadobj(objectid_t objid) {
    tSimObj* retval = nullptr;
    //
    //  We call the simcore to load an object.
    tSigObjectLoadReqPtr   loadreq = newPtr (tSigObjectLoadReq);
    //
    //  Creation of the two endpoints.
    std::future<tSQLRecordSet*>  loadchannel;
    std::promise<tSQLRecordSet*> promise;

    loadchannel = promise.get_future();
    //
    //  The promise we send with the message.
    loadreq->loadchannel = std::move(promise);
    loadreq->objectid    = objid;
    loadreq->handle      = __coreIfc->stdbhandle;

    __coreIfc->stdb->Put(loadreq);
    //
    //  Wait until the message gets answered.
    tSQLRecordSet* data = loadchannel.get();

    if (data != nullptr) {

    }

    return retval;
}

void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, double value) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueDouble;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.dbl=value;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, double value) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueDouble;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.dbl=value;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_deletefloatdata(objectid_t objid, valueid_t valueid, valueindex_t index) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueDouble;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->value.dbl=0;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, uint64_t value) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.u64=value;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, uint64_t value) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.u64=value;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, bool value) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.u64=value;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, bool value) {
    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.u64=value;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, int64_t value) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueI64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.i64=value;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, int64_t value) {
    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueI64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.i64=value;

        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_deleteintdata(objectid_t objid, valueid_t valueid, valueindex_t index) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueI64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->value.i64 = 0;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_createdata(objectid_t objid, valueid_t valueid, valueindex_t index, const char*value) {
    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueString;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        if (value != 0) {
            msg->value.str=strdup(value);
        } else {
            msg->value.str=strdup("<null>");
        }

        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_updatedata(objectid_t objid, valueid_t valueid, valueindex_t index, const char*value) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueString;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        if (value != 0) {
            msg->value.str=strdup(value);
        } else {
            msg->value.str=strdup("<null>");
        }
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_deletetextdata(objectid_t objid, valueid_t valueid, valueindex_t index) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueString;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->value.str=strdup("<null>");

        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_createreferencedata(objectid_t objid, valueid_t valueid, valueindex_t index, objectid_t reference) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueReference;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.u64=reference;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_updatereferencedata(objectid_t objid, valueid_t valueid, valueindex_t index, objectid_t reference) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueReference;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.u64=reference;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_deletereferencedata(objectid_t objid, valueid_t valueid, valueindex_t index) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        msg->id=eMsgDBValueReference;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->value.u64=0;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}


void stdb_set(uint8_t attrtype, objectid_t objid, valueid_t valueid, valueindex_t index, const tVariant& aValue) {
    if (__coreIfc->stdb != 0) {
        tMsgDBSetPtr msg = std::make_shared<tMsgDBSet>(attrtype, objid, valueid, index, aValue);
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_del(uint8_t attrtype, objectid_t objid, valueid_t valueid, valueindex_t index) {

    if (__coreIfc->stdb != 0) {
        tMsgDBValuePtr msg = newPtr (tMsgDBValue);

        switch(attrtype) {
        case 0x01:
            msg->id=eMsgDBValueDouble;
            break;
        case 0x02:
            msg->id=eMsgDBValueI64;
            break;
        case 0x04:
            msg->id=eMsgDBValueString;
            break;
        case 0x08:
            msg->id=eMsgDBValueReference;
            break;
        default:
            break;
        }

        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->handle = __coreIfc->stdbhandle;
        __coreIfc->stdb->Put(msg);
    }
}

void stdb_sqlexecstring(tCommTarget sender, uint64_t cmdtype, const char *cmd, uint64_t limit) {
    tSigSQLExecStringReqPtr msg = newPtr (tSigSQLExecStringReq);

    msg->src     = sender;
    msg->handle  = __coreIfc->stdbhandle;
    msg->src     = sender;
    msg->CmdType = cmdtype;
    msg->Limit   = limit;
    msg->SQL     = cmd;

    __coreIfc->stdb->Put(msg);
}

void stdb_fetch(tCommTarget sender, uint64_t cmdType, uint64_t limit) {
    tSigSQLFetchReqPtr msg = newPtr (tSigSQLFetchReq);

    msg->src     = sender;
    msg->cmdType = cmdType;
    msg->handle  = __coreIfc->stdbhandle;
    msg->Limit   = limit;

    __coreIfc->stdb->Put(msg);
}


#define DB_OPEN(db) \
void open(tCommTarget sender, std::string name, std::string host, std::string port, std::string user, std::string passwd) {\
    tSigSQLConnectReqPtr msg = newPtr (tSigSQLConnectReq);\
\
    msg->src   = sender;\
    msg->name  = name;\
    msg->host  = host;\
    msg->port  = port;\
    msg->user  = user;\
    msg->passwd = passwd;\
\
    db->Put(msg);\
}

#define DB_EXECSTRING(db)\
void sqlexecstring(tCommTarget sender, int handle, uint64_t cmdtype, const char* cmd, uint32_t limit) {\
    tSigSQLExecStringReqPtr msg= newPtr (tSigSQLExecStringReq);\
\
    msg->src     = sender;\
    msg->handle  = handle;\
    msg->src     = sender;\
    msg->CmdType = cmdtype;\
    msg->Limit   = limit;\
    msg->SQL     = cmd;\
\
    db->Put(msg);\
}

#define DB_FETCH(db)\
void sqlfetch(tCommTarget sender, int handle, uint32_t limit) {\
    tSigSQLFetchReqPtr msg = newPtr (tSigSQLFetchReq);\
\
    msg->src    = sender;\
    msg->handle = handle;\
    msg->Limit  = limit;\
\
    db->Put(msg);\
}

#define DB_CLOSE(db)\
void close(tCommTarget sender, int handle) {\
    tSigSQLDisconnectReqPtr msg = newPtr (tSigSQLDisconnectReq);\
\
    msg->src    = sender;\
    msg->handle = handle;\
\
    db->Put(msg);\
}

namespace stdb {
    DB_OPEN(__coreIfc->stdb)
    DB_EXECSTRING(__coreIfc->stdb)
    DB_FETCH(__coreIfc->stdb)
    DB_CLOSE(__coreIfc->stdb)
}

namespace pgdb {
    DB_OPEN(__coreIfc->pgdb)
    DB_EXECSTRING(__coreIfc->pgdb)
    DB_FETCH(__coreIfc->pgdb)
    DB_CLOSE(__coreIfc->pgdb)
}

namespace mariadb {
    DB_OPEN(__coreIfc->mariadb)
    DB_EXECSTRING(__coreIfc->mariadb)
    DB_FETCH(__coreIfc->mariadb)
    DB_CLOSE(__coreIfc->mariadb)
}

objectid_t to_objectid_t(tJSON *j) {
    objectid_t retval = 0ul;

    if (j != nullptr) {
        retval = static_cast<objectid_t>(to_uint64_t(j));
    }
    return retval;
}

