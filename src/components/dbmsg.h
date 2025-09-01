// **************************************************************************
//
//  Modul-Name     : dbmsg.h
//  Author(s)      : Hans-Juergen Lange
//
//  Copyrights 2014 by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef DBMSG_H
#define DBMSG_H

#include <msg.h>
#include <msgdb.h>
#include <variant.h>
//
// other db messages.
#include <tMsgSQLExecStringReply.h>
#include <tMsgSQLExecStringReq.h>
#include <tSigSQLExecStringReply.h>
#include <tSigSQLExecStringReq.h>
#include <tSigSQLConnectReply.h>
#include <tSigSQLConnectReq.h>
#include <tSigSQLDisconnectReply.h>
#include <tSigSQLDisconnectReq.h>
#include <tSigSQLFetchReply.h>
#include <tSigSQLFetchReq.h>
#include <tSigPrepareStorageReply.h>
#include <tSigPrepareStorageReq.h>

typedef enum enumDBMsgIds {
    eMsgDBTransactionStart=0x0f00,
    eMsgDBTransactionEnd,
    eMsgDBDeleteObject,
    eMsgDBCreateObject,
    eMsgDBCreateFromTemplate,
    eMsgDBValueDouble,
    eMsgDBValueU64,
    eMsgDBValueI64,
    eMsgDBValueString,
    eMsgDBValueReference,
    eMsgDBSet,
    eMsgDBNAK
} eDBMsgIds;

typedef enum enumValueAction {
    eValueCreate,
    eValueDelete,
    eValueUpdate
} eValueAction;

/*
 * Here are some messages used with the TLog mechanism.
 * Some of them are still used to update the Simulation DB
 */
struct tMsgDBTransactionStart : public tMsgDB {
    tMsgDBTransactionStart() {id = eMsgDBTransactionStart; }
    virtual ~tMsgDBTransactionStart();
};

using tMsgDBTransactionStartPtr = std::shared_ptr<tMsgDBTransactionStart>;

struct tMsgDBTransactionEnd : public tMsgDB {
    tMsgDBTransactionEnd() {id = eMsgDBTransactionEnd;}
    virtual ~tMsgDBTransactionEnd();
};

using tMsgDBTransactionEndPtr = std::shared_ptr<tMsgDBTransactionEnd>;

struct tMsgDBValue : public tMsgDB {
    tMsgDBValue() {}
    virtual ~tMsgDBValue() {
        if (id == eMsgDBValueString) {
            if (value.str != nullptr) {
                //
                //  As this message gets send from the simapi it has been allocated
                //  through strdup() that uses malloc. So we have to use free here.
                free(value.str);
            }
        }
    }
    uint64_t   objid;             //  The Object ID
    uint64_t    valueid;           //  Value ID
    uint64_t index;             //  The index on array values.
    eValueAction action;        //  What todo. Create/Delete/Update
    union {
        double dbl;
        uint64_t u64;
        int64_t  i64;
        char *str;
    } value;
};

using tMsgDBValuePtr = std::shared_ptr<tMsgDBValue>;

struct tMsgDBDeleteObject : public tMsgDB {
    tMsgDBDeleteObject() {}
    virtual ~tMsgDBDeleteObject() {}
    uint64_t objectid;
};

using tMsgDBDeleteObjectPtr = std::shared_ptr<tMsgDBDeleteObject>;

struct tMsgDBCreateObject : public tMsgDB {
    tMsgDBCreateObject() {}
    virtual ~tMsgDBCreateObject() {}
    uint64_t   objectid;
    uint64_t   type;
    uint64_t   droptime;
} ;

using tMsgDBCreateObjectPtr = std::shared_ptr<tMsgDBCreateObject>;

struct tMsgDBCreateFromTemplate : public tMsgDB {
    tMsgDBCreateFromTemplate() {}
    virtual ~tMsgDBCreateFromTemplate() {}
    uint64_t templateid;
    uint64_t objectid;
    uint64_t type;
};

using tMsgDBCreateFromTemplatePtr = std::shared_ptr<tMsgDBCreateFromTemplate>;

struct tMsgDBSet : tMsgDB {
    tMsgDBSet() {id = eMsgDBSet;}
    tMsgDBSet(uint8_t aAttrType, uint64_t aOID, uint64_t aVID, uint64_t aVIDX, const tVariant& aValue) {
        attrtype = aAttrType; oid = aOID; vid = aVID; vidx = aVIDX; value = aValue; id = eMsgDBSet;
    }
    virtual ~tMsgDBSet() {}
    uint8_t  attrtype;
    uint64_t oid;
    uint64_t vid;
    uint64_t vidx;
    tVariant value;
};

struct tMsgDBNAK : public tMsgDB {
    tMsgDBNAK() {id = eMsgDBNAK; }
    virtual ~tMsgDBNAK() = default;
};

using tMsgDBSetPtr = std::shared_ptr<tMsgDBSet>;

#define MSGMAP std::map<uint64_t, std::string> stdbmsg = {\
{eMsgDBTransactionStart,   "DBTransactionStart"},\
{eMsgDBTransactionEnd,     "DBTransactionEnd"},\
{eMsgDBDeleteObject,       "DBDeleteObject"},\
{eMsgDBCreateObject,       "DBCreateObject"},\
{eMsgDBCreateFromTemplate, "DBCreateFromTemplate"},\
{eMsgDBValueDouble,        "DBValueDouble"},\
{eMsgDBValueU64,           "DBValueU64"},\
{eMsgDBValueI64,           "DBValueI64"},\
{eMsgDBValueString,        "DBValueString"},\
{eMsgDBValueReference,     "DBValueReference"},\
{eMsgDBSet,                "DBSet"},\
{IDM_SQLEXECSTRINGREPLY ,  "SyncSQLExecStringReply"},\
{IDM_SQLEXECSTRINGREQ ,    "SyncSQLExecStringReq"},\
{IDS_PREPARESTORAGEREPLY , "PrepareStorageReply"},\
{IDS_PREPARESTORAGEREQ ,   "PrepareStorageReq"},\
{IDS_SQLCONNECTREPLY ,     "SQLConnectReply"},\
{IDS_SQLCONNECTREQ ,       "SQLConnectReq"},\
{IDS_SQLDISCONNECTREPLY ,  "SQLDisconnectReply"},\
{IDS_SQLDISCONNECTREQ ,    "SQLDisconnectReq"},\
{IDS_SQLEXECSTRINGREPLY ,  "AsyncSQLExecStringReply"},\
{IDS_SQLEXECSTRINGREQ ,    "AsyncSQLExecStringReq"},\
{IDS_SQLFETCHREPLY ,       "SQLFetchReply"},\
{IDS_SQLFETCHREQ ,         "SQLFetchReq"}\
};

#endif // DBMSG_H
