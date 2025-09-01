#include <dbmsg.h>
#include <tSigSQLExecStringReq.h>
#include <tSigSQLConnectReq.h>
#include <tSigSQLDisconnectReq.h>
#include <tSigSQLFetchReq.h>
#include <tSigPrepareStorageReq.h>
#include <CEvent.h>
#include <sharedmsgqueue.h>


namespace stdb {

CSharedMsgQueue* db = nullptr;

void createobj(int handle, uint64_t objid, uint64_t type) {
    if (db != nullptr) {
        tMsgDBCreateObjectPtr msg = newPtr(tMsgDBCreateObject);

        msg->handle   = handle;
        msg->id       = eMsgDBCreateObject;
        msg->objectid = objid;
        msg->type     = type;

        db->Put(msg);
    } else {

    }
}

void createfromtemplate(int handle, uint64_t templateid, uint64_t objid, uint64_t type) {
    if (db != nullptr) {
        tMsgDBCreateFromTemplatePtr msg = newPtr(tMsgDBCreateFromTemplate);

        msg->handle   = handle;
        msg->id=eMsgDBCreateFromTemplate;
        msg->templateid=templateid;
        msg->objectid=objid;
        msg->type=type;

        db->Put(msg);
    } else {

    }
}

void deleteobj(int handle, uint64_t objid) {
    if (db != nullptr) {
        tMsgDBDeleteObjectPtr msg = newPtr(tMsgDBDeleteObject);

        msg->handle   = handle;
        msg->id=eMsgDBDeleteObject;
        msg->objectid=objid;

        db->Put(msg);
    } else {

    }
}

void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, double value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueDouble;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.dbl=value;

        db->Put(msg);
    } else {

    }
}

void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, double value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueDouble;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.dbl=value;

        db->Put(msg);
    } else {

    }
}

void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, double value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueDouble;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->value.dbl=value;

        db->Put(msg);
    } else {

    }
}

void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.u64=value;

        db->Put(msg);
    } else {

    }
}

void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.u64=value;

        db->Put(msg);
    } else {

    }
}

void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->value.u64=value;

        db->Put(msg);
    } else {

    }
}
void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, bool value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.u64=value;

        db->Put(msg);
    } else {

    }
}

void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, bool value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.u64=value;

        db->Put(msg);
    } else {

    }
}

void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, bool value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueU64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->value.u64=value;

        db->Put(msg);
    } else {

    }
}

void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, int64_t value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueI64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.i64=value;

        db->Put(msg);
    } else {

    }
}

void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, int64_t value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueI64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.i64=value;

        db->Put(msg);
    } else {

    }
}

void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, int64_t value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueI64;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->value.i64=value;

        db->Put(msg);
    } else {

    }
}

void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, const char*value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
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

        db->Put(msg);
    } else {

    }
}

void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, const char*value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
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

        db->Put(msg);
    } else {

    }
}

void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, const char*value) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueString;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        if (value != 0) {
            msg->value.str=strdup(value);
        } else {
            msg->value.str=strdup("<null>");
        }

        db->Put(msg);
    } else {

    }
}

void createreferencedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t reference) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueReference;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueCreate;
        msg->value.u64=reference;

        db->Put(msg);
    } else {

    }
}

void updatereferencedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t reference) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueReference;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueUpdate;
        msg->value.u64=reference;

        db->Put(msg);
    } else {

    }
}

void deletereferencedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index) {
    if (db != nullptr) {
        tMsgDBValuePtr msg = newPtr(tMsgDBValue);

        msg->handle   = handle;
        msg->id=eMsgDBValueReference;
        msg->objid=objid;
        msg->valueid=valueid;
        msg->index=index;
        msg->action=eValueDelete;
        msg->value.u64=0;

        db->Put(msg);
    } else {

    }
}

void preparestorage(tCommTarget sender, int handle, bool history, bool versions, bool ondemand) {
    if (db != nullptr) {
        tSigPrepareStorageReqPtr msg= newPtr(tSigPrepareStorageReq);

        msg->src     = sender;
        msg->handle  = handle;
        msg->history = history;
        msg->versions = versions;
        msg->objectsondemand = ondemand;

        db->Put(msg);
    } else {

    }
}

}  //  namespace stdb-end

#define DB_OPEN() void open(tCommTarget sender, std::string name, std::string host, std::string port, std::string user, std::string passwd) {\
    if (db != nullptr) {\
        tSigSQLConnectReqPtr msg = newPtr(tSigSQLConnectReq);\
\
        msg->src   = sender;\
        msg->name  = name;\
        msg->host  = host;\
        msg->port  = port;\
        msg->user  = user;\
        msg->passwd = passwd;\
\
        db->Put(msg);\
    } else {\
    }\
}\


#define DB_EXECSTRING() void sqlexecstring(tCommTarget sender, int handle, uint64_t cmdtype, const char* cmd, uint32_t limit) {\
    if (db != nullptr) {\
        tSigSQLExecStringReqPtr msg = newPtr(tSigSQLExecStringReq);\
\
        msg->src     = sender;\
        msg->handle  = handle;\
        msg->src     = sender;\
        msg->CmdType = cmdtype;\
        msg->Limit   = limit;\
        msg->SQL     = strdup(cmd);\
\
        db->Put(msg);\
    } else {\
    }\
}\


#define DB_FETCH() void sqlfetch(tCommTarget sender, int handle, uint32_t limit) {\
    if (db != nullptr) {\
        tSigSQLFetchReqPtr msg = newPtr(tSigSQLFetchReq);\
\
        msg->src    = sender;\
        msg->handle = handle;\
        msg->Limit  = limit;\
\
        db->Put(msg);\
    } else {\
    }\
}\


#define DB_CLOSE() void close(tCommTarget sender, int handle) {\
    if (db != nullptr) {\
        tSigSQLDisconnectReqPtr msg = newPtr(tSigSQLDisconnectReq);\
\
        msg->src    = sender;\
        msg->handle = handle;\
\
        db->Put(msg);\
    } else {\
    }\
}\

namespace stdb {
DB_OPEN()
DB_EXECSTRING()
DB_FETCH()
DB_CLOSE()
}

namespace mariadb  {
CSharedMsgQueue* db = nullptr;
DB_OPEN()
DB_EXECSTRING()
DB_FETCH()
DB_CLOSE()
}

namespace pgdb {
CSharedMsgQueue* db = nullptr;
DB_OPEN()
DB_EXECSTRING()
DB_FETCH()
DB_CLOSE()
}
