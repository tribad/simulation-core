// MIT License
//
// Copyright (c) 2014 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <endian.h>
#include <sstream>
#include "msg.h"
#include "objdb.h"
#include "logger.h"
//
//  PostgresDB connection object
PGresult *objlist;
PGresult *doublelist;
PGresult *integerlist;
//PGresult *textlist;
PGresult *referencelist;

PGresult *createobject;
PGresult *createfromtemplate;
PGresult *deleteobject;

PGresult *createdoublevalue;
PGresult *updatedoublevalue;
PGresult *setdoublevalue;
PGresult *deletedoublevalue;

PGresult *createintevalue;
PGresult *updateintvalue;
PGresult *setintvalue;
PGresult *deleteintvalue;

PGresult *createstringevalue;
PGresult *updatestringvalue;
PGresult *setstringvalue;
PGresult *deletestringvalue;

PGresult *createreferenceevalue;
PGresult *updatereferencevalue;
PGresult *setreferencevalue;
PGresult *deletereferencevalue;

bool tPG::ResCommandOK(const PGresult* res, const char* cmd) {
    bool           retval = true;
    ExecStatusType pqerr;

    pqerr = PQresultStatus(res);
    if (pqerr != PGRES_COMMAND_OK) {
        char* errmsg = PQresultErrorMessage(res);

        retval = false;
        if (errmsg != nullptr) {
            pgsqllogger.Write(LogLevel::Error, "%s : %s : %s", cmd, PQresStatus(pqerr), errmsg);
        } else {
            pgsqllogger.Write(LogLevel::Error, "%s : %s", cmd, PQresStatus(pqerr));
        }
    }
    return retval;
}

bool tPG::ResTuplesOK(const PGresult* res, const char* cmd) {
    bool           retval = true;
    ExecStatusType pqerr;

    pqerr = PQresultStatus(res);
    if (pqerr != PGRES_TUPLES_OK) {
        char* errmsg = PQresultErrorMessage(res);

        retval = false;
        if (errmsg != nullptr) {
            pgsqllogger.Write(LogLevel::Error, "%s : %s : %s", cmd, PQresStatus(pqerr), errmsg);
        } else {
            pgsqllogger.Write(LogLevel::Error, "%s : %s", cmd, PQresStatus(pqerr));
        }
    }
    return retval;
}

void tPG::WaitForReconnect() {
    state = PGStatus::waitphase;
    reconnect = time(0) + RECONNECT_TIMEOUT;
    //
    //  Free all resources of the pg-lib.
    db_reset();
}

tPG::tPG(CLogger& logger, std::string aName, std::string aHost, std::string aPort, std::string aUser, std::string aPasswd) : pgsqllogger(logger) {
    state       = PGStatus::disconnected;
    queryresult = 0;
    position    = 0;
    name        = aName;
    host        = aHost;
    port        = aPort;
    user        = aUser;
    passwd      = aPasswd;
    reconnect   = 0;

    dbconnect(name, user, passwd, port, host);
    if (db != nullptr) {
        state = PGStatus::connected;
    } else {
        WaitForReconnect();
    }
}

bool tPG::IsReadyForReconnect() {
    bool retval = false;
    //
    //  Reset of the time will be done in an action
    //  because the reconnect time has been reached.
    if ((reconnect != 0u) && (time(nullptr) >= reconnect)) {
        retval = true;
    }
    return retval;
}

void tPG::DoReconnect() {

    dbconnect(name, user, passwd, port, host);

    if (db != 0) {
        state     = PGStatus::connected;
        reconnect = 0;

        for (auto p : prepared) {
            p.second.prep = PQprepare(db, p.first.c_str(), p.second.cmd.c_str() , p.second.pcount, 0 );

            ResCommandOK(p.second.prep, p.second.cmd.c_str());
        }
    } else {
        //
        //  Re-Arm the reconnect timer.
        WaitForReconnect();
    }
}
tPG::~tPG() {
    //
    //  If we have any query result outstanding.
    if (queryresult != 0) {
        PQclear(queryresult);
    }
    //
    //  release the prepared statements.
    for (auto p : prepared) {
        PQclear(p.second.prep);
    }
    prepared.clear();

    db_disconnect();
    reconnect = 0;
    state     = PGStatus::disconnected;
}



tMsg* tPG::dbsqldisconnectreq(tSigSQLDisconnectReqPtr msg) {
    tSigSQLDisconnectReply*       reply = new tSigSQLDisconnectReply;

    reply->dst    = msg->src;
    reply->handle = msg->handle;

    return reply;
}

tMsg* tPG::dbsqlfetchreq(tSigSQLFetchReqPtr sig) {
    tSQLRecordSet*                result;
    tSigSQLFetchReply*            reply    = nullptr;

    if (IsConnected()) {
        reply    = new tSigSQLFetchReply;

        result = db_sqlfetch(sig->Limit);

        if (result != nullptr) {
            reply->RecordSet = result;
            reply->Error     = result->Error;
        } else {
            reply->RecordSet =  nullptr;
            reply->Error     = -1;
        }

        reply->dst      = sig->src;
        reply->src      = tDBQueue();
        reply->cmdType  = sig->cmdType;

    } else {
        temp.push_back(sig);
    }

    return reply;
}
//
//  Delete an object with all its attributes from the DB.
tMsg* tPG::dbdelobj(tMsgDBDeleteObjectPtr msg) {
    PGresult*                     ret = nullptr;

    if (IsConnected()) {
        ret = db_deleteobject(msg->objectid);
        //
        //  If the request runs ok we can delete the message.
        if (ret != nullptr) {
            PQclear(ret);
        } else {
            //
            //  Setup wait phase and push the message into the wait-queue.
            WaitForReconnect();

            temp.push_back(msg);
        }

    } else {
        temp.push_back(msg);
    }

    return nullptr;
}
//
//  Create  a new object in the Postgres DB.
tMsg* tPG::dbcreateobj(tMsgDBCreateObjectPtr msg) {
    PGresult*                     ret = nullptr;

    if (IsConnected()) {
        ret = db_createobject(msg->objectid, msg->type, msg->droptime);

        if (ret != nullptr) {
            PQclear(ret);
        } else {
            //
            //  Setup wait phase and push the message into the wait-queue.
            WaitForReconnect();

            temp.push_back(msg);
        }
    } else {
        temp.push_back(msg);
    }

    return nullptr;
}
//
//  Create  a new object from a template in the Postgres DB.
tMsg* tPG::dbcreatefromtemplate(tMsgDBCreateFromTemplatePtr msg) {
    PGresult*                     ret = nullptr;

    if (IsConnected()) {
        ret = db_createfromtemplate(msg->templateid, msg->type, msg->objectid);

        if (ret != nullptr) {
            PQclear(ret);
        } else {
            //
            //  Setup wait phase and push the message into the wait-queue.
            WaitForReconnect();

            temp.push_back(msg);
        }
    } else {
        temp.push_back(msg);
    }
    return nullptr;
}
//
//  Create/Delete or Update a value in the postgres DB.
tMsg* tPG::dbdoublevalue(tMsgDBValuePtr msg) {
    PGresult*                     ret = nullptr;

    if (IsConnected()) {
        switch (msg->action) {
        case eValueCreate:
            ret = db_createdoublevalue(msg->objid, msg->valueid, msg->index, msg->value.dbl);
            break;
        case eValueUpdate:
            ret = db_updatedoublevalue(msg->objid, msg->valueid, msg->index, msg->value.dbl);
            break;
        case eValueDelete:
            ret = db_deletedoublevalue(msg->objid, msg->valueid, msg->index);
            break;
        default:
            break;
        }

        if (ret != nullptr) {
            PQclear(ret);
        } else {
            //
            //  Setup wait phase and push the message into the wait-queue.
            WaitForReconnect();

            temp.push_back(msg);
        }
    } else {
        temp.push_back(msg);
    }
    return nullptr;
}
//
//  Create/Delete or Update a value in the postgres DB.
tMsg* tPG::dbintvalue(tMsgDBValuePtr msg) {
    PGresult*                     ret = nullptr;

    if (IsConnected()) {
        switch (msg->action) {
        case eValueCreate:
            ret = db_createintvalue(msg->objid, msg->valueid, msg->index, msg->value.i64);
            break;
        case eValueUpdate:
            ret = db_updateintvalue(msg->objid, msg->valueid, msg->index, msg->value.i64);
            break;
        case eValueDelete:
            ret = db_deleteintvalue(msg->objid, msg->valueid, msg->index);
            break;
        default:
            break;
        }

        if (ret != nullptr) {
            PQclear(ret);
        } else {
            //
            //  Setup wait phase and push the message into the wait-queue.
            WaitForReconnect();

            temp.push_back(msg);
        }
    } else {
        temp.push_back(msg);
    }
    return nullptr;
}
//
//  Create/Delete or Update a value in the postgres DB.
tMsg* tPG::dbstringvalue(tMsgDBValuePtr msg) {
    PGresult*                     ret = nullptr;

    if (IsConnected()) {
        switch (msg->action) {
        case eValueCreate:
            ret = db_createstringvalue(msg->objid, msg->valueid, msg->index, msg->value.str);
            break;
        case eValueUpdate:
            ret = db_updatestringvalue(msg->objid, msg->valueid, msg->index, msg->value.str);
            break;
        case eValueDelete:
            ret = db_deletestringvalue(msg->objid, msg->valueid, msg->index);
            break;
        default:
            break;
        }

        if (ret != nullptr) {
            PQclear(ret);
        } else {
            //
            //  Setup wait phase and push the message into the wait-queue.
            WaitForReconnect();

            temp.push_back(msg);
        }
    } else {
        temp.push_back(msg);
    }
    return nullptr;
}
//
//  Create/Delete or Update a value in the postgres DB.
tMsg* tPG::dbreferencevalue(tMsgDBValuePtr msg) {
    PGresult*                     ret = nullptr;

    if (IsConnected()) {
        switch (msg->action) {
        case eValueCreate:
            ret = db_createreferencevalue(msg->objid, msg->valueid, msg->index, msg->value.u64);
            break;
        case eValueUpdate:
            ret = db_updatereferencevalue(msg->objid, msg->valueid, msg->index, msg->value.u64);
            break;
        case eValueDelete:
            ret = db_deletereferencevalue(msg->objid, msg->valueid, msg->index);
            break;
        default:
            break;
        }

        if (ret != nullptr) {
            PQclear(ret);
        } else {
            //
            //  Setup wait phase and push the message into the wait-queue.
            WaitForReconnect();

            temp.push_back(msg);
        }
    } else {
        temp.push_back(msg);
    }
    return nullptr;
}

tMsg* tPG::dbsetvalue(tMsgDBSetPtr msg) {
    PGresult*                     ret = nullptr;

    if (IsConnected()) {
        switch(msg->attrtype) {
        case 0x01:
            ret = db_setdblvalue(msg->oid, msg->vid, msg->vidx, msg->value);
            break;
        case 0x02:
            ret = db_setintvalue(msg->oid, msg->vid, msg->vidx, msg->value);
            break;
        case 0x04:
            ret = db_settextvalue(msg->oid, msg->vid, msg->vidx, ((std::string)(msg->value)).c_str());
            break;
        case 0x08:
            ret = db_setrefvalue(msg->oid, msg->vid, msg->vidx, (uint64_t)(msg->value));
            break;
        default:
            return nullptr;
        }
        if (ret != nullptr) {
            PQclear(ret);
        } else {
            //
            //  Setup wait phase and push the message into the wait-queue.
            WaitForReconnect();

            temp.push_back(msg);
        }
    } else {
        temp.push_back(msg);
    }
    return nullptr;
}

tMsg* tPG::dbsqlexecstringreq(tSigSQLExecStringReqPtr sig) {
    tSQLRecordSet*                result = nullptr;
    tSigSQLExecStringReply*       reply  = nullptr;

    if (IsConnected()) {
        reply  = new tSigSQLExecStringReply;
        if (!sig->SQL.empty()) {
            result = db_sqlexecstring(sig->SQL.c_str(), sig->Limit);
        } else {
            pgsqllogger.Write(LogLevel::Error, "Invalid SQL Exec String : <nullptr>");
        }

        if (result != nullptr) {
            reply->RecordSet = result;
            reply->Error     = result->Error;
        } else {
            reply->RecordSet =  nullptr;
            reply->Error     = -1;
        }

        reply->dst      = sig->src;
        reply->CmdType  = sig->CmdType;
        reply->src      = tDBQueue();
    } else {
        temp.push_back(sig);
    }
    return reply;
}

tMsg* tPG::dbprepstorage(tSigPrepareStorageReqPtr sig) {
    tSigPrepareStorageReply* reply = nullptr;

    if (IsConnected()) {
        reply = new tSigPrepareStorageReply;
        db_preparestorage();

        reply->dst   = sig->src;
        reply->Error = 0;

        reply->src      = tDBQueue();
    } else {
        temp.push_back(sig);
    }
    return reply;
}

tMsg* tPG::dbobjectloadreq(tSigObjectLoadReqPtr msg) {
    tMsg* reply = nullptr;

    if (IsConnected()) {
         tSQLRecordSet*                result = nullptr;
         std::ostringstream oss;

         oss << "select * from getrawobject(" << msg->objectid << ");";
         result = db_sqlexecstring(oss.str().c_str(), UINT32_MAX);

         msg->loadchannel.set_value(result);
    } else {
        temp.push_back(msg);
    }
    return reply;
}


void tPG::db_preparestorage() {
    tPreparedStatement statement;

    statement.cmd    = "select * from public.Objects where typeid=$1;";
    statement.pcount = 1;
    statement.prep   = PQprepare(db, "objectsbytype", statement.cmd.c_str() , 1, 0 );
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("objectsbytype", statement));

    statement.cmd  = "select valueid, index, value from public.doublevalue where objectid=$1;";
    statement.pcount = 1;
    statement.prep = PQprepare(db, "objectdoublevalues", statement.cmd.c_str(), 1, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("objectdoublevalues", statement));

    statement.cmd  = "select valueid, index, value from public.integervalue where objectid=$1;";
    statement.pcount = 1;
    statement.prep = PQprepare(db, "objectintegervalues", statement.cmd.c_str(), 1, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("objectintegervalues", statement));

    statement.cmd  = "select valueid, index, text from public.textvalue where objectid=$1;";
    statement.pcount = 1;
    statement.prep = PQprepare(db, "objecttextvalues", statement.cmd.c_str(), 1, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("objecttextvalues", statement));

    statement.cmd  = "select valueid, index, other from public.refs where objectid=$1;";
    statement.pcount = 1;
    statement.prep = PQprepare(db, "objectrefsvalues", statement.cmd.c_str(), 1, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("objectrefsvalues", statement));


    statement.cmd  = "insert into doublevalue values($1, $2, $3, $4);";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "createdoublevalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("createdoublevalue", statement));

    statement.cmd  = "update doublevalue set value=$4 where objectid=$1 and valueid=$2 and index=$3;";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "updatedoublevalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("updatedoublevalue", statement));

    statement.cmd  = "select setdouble ($1, $2, $3, $4);";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "setdoublevalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("setdoublevalue", statement));

    statement.cmd  = "delete from doublevalue where objectid=$1 and valueid=$2 and index=$3;";
    statement.pcount = 3;
    statement.prep = PQprepare(db, "deletedoublevalue", statement.cmd.c_str(), 3, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("deletedoublevalue", statement));


    statement.cmd  = "insert into integervalue values($1, $2, $3, $4);";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "createintvalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("createintvalue", statement));

    statement.cmd  = "update integervalue set value=$4 where objectid=$1 and valueid=$2 and index=$3;";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "updateintvalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("updateintvalue", statement));

    statement.cmd  = "select setinteger($1, $2, $3, $4);";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "setintvalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("setintvalue", statement));

    statement.cmd  = "delete from integervalue where objectid=$1 and valueid=$2 and index=$3;";
    statement.pcount = 3;
    statement.prep = PQprepare(db, "deleteintvalue", statement.cmd.c_str(), 3, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("deleteintvalue", statement));


    statement.cmd  = "insert into textvalue values($1, $2, $3, $4);";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "createstringvalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("createstringvalue", statement));

    statement.cmd  = "update textvalue set text=$4 where objectid=$1 and valueid=$2 and index=$3;";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "updatestringvalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("updatestringvalue", statement));

    statement.cmd  = "select setstring($1, $2, $3, $4);";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "setstringvalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("setstringvalue", statement));

    statement.cmd  = "delete from textvalue where objectid=$1 and valueid=$2 and index=$3;";
    statement.pcount = 3;
    statement.prep = PQprepare(db, "deletestringvalue", statement.cmd.c_str(), 3, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("deletestringvalue", statement));


    statement.cmd  = "insert into refs values($1, $2, $3, $4);";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "createreferencevalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("createreferencevalue", statement));

    statement.cmd  = "update refs set other=$4 where objectid=$1 and valueid=$2 and index=$3;";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "updatereferencevalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("updatereferencevalue", statement));

    statement.cmd  = "select setreference($1, $2, $3, $4);";
    statement.pcount = 4;
    statement.prep = PQprepare(db, "setreferencevalue", statement.cmd.c_str(), 4, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("setreferencevalue", statement));

    statement.cmd  = "delete from refs where objectid=$1 and valueid=$2 and index=$3;";
    statement.pcount = 3;
    statement.prep = PQprepare(db, "deletereferencevalue", statement.cmd.c_str(), 3, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("deletereferencevalue", statement));


    statement.cmd  = "insert into objects values($1, $2, $3);";
    statement.pcount = 3;
    statement.prep = PQprepare(db, "createobject", statement.cmd.c_str(), 3, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("createobject", statement));
#if 0
    statement.cmd  = "select createfromtemplate($1, $2, $3);";
    statement.prep = PQprepare(db, "createfromtemplate", statement.cmd.c_str(),3,0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("createfromtemplate", statement));
#endif
    statement.cmd  = "select deleteobject($1);";
    statement.pcount = 1;
    statement.prep = PQprepare(db, "deleteobject", statement.cmd.c_str(), 1, 0);
    ResCommandOK(statement.prep, statement.cmd.c_str());
    prepared.insert(std::pair<std::string, tPreparedStatement>("deleteobject", statement));
}

void tPG::dbconnect(const std::string& dbname, const std::string& user, const std::string& password, const std::string& port, const std::string& host) {
    std::ostringstream oss;

    if (!dbname.empty()) {
        oss << "dbname = " << dbname;
    } else {
        pgsqllogger.Write(LogLevel::Error, "No dbname set");
        db = nullptr;
    }
    oss << " host = ";
    if (!host.empty()) {
        oss << host;
    } else {
        pgsqllogger.Write(LogLevel::Warn, "No dbhost set using localhost");
        oss << "localhost";
    }
    if (!user.empty()) {
        oss << " user = " << user;
    }
    if (!port.empty()) {
        oss << " port = " << port;
    } else {
        pgsqllogger.Write(LogLevel::Info, "No dbport set using default");
    }
    //
    //  display message here because the password will be set next. So it does not gets displayed.
    pgsqllogger.Write(LogLevel::Info, "Using this connectionstring: %s : password omitted here.", oss.str().c_str());
    //
    //  Now add the password.
    if (!password.empty()) {
        oss << " password = " << password;
    }
    db = PQconnectdb(oss.str().c_str());
    //
    //  Because of the synchronous nature of the PQconnectdb we only need to check for
    //  CONNECTION_OK
    if (PQstatus(db) != CONNECTION_OK)
    {
        pgsqllogger.Write(LogLevel::Error, "Connect failed: %s", PQerrorMessage(db));
        PQfinish(db);
        db = nullptr;
    } else {
        pgsqllogger.Write(LogLevel::Info,  "Connected");
    }
}

void tPG::db_disconnect() {
    db_reset();
    prepared.clear();
}

void tPG::db_reset() {
    //
    //  All pg specifc structure need to be freed
    std::map<std::string, tPreparedStatement>::iterator pi;

    for (pi = prepared.begin(); pi != prepared.end(); ++ pi) {
        PQclear(pi->second.prep);
        pi->second.prep = nullptr;
    }
    //
    //  The db gets closed.
    PQfinish(db);
    db = nullptr;
    //
    //  We do not change the state of the tPG object.
}


#if 0
int dbloadobj(PGconn *prbgdb, tSimObj *obj) {
    const char*        parametervalue[1];
    int                length[1] = {sizeof(uint64_t)};
    int                pformat[] = {1};
    uint64_t         p1        = htobe64(obj->objid);

    parametervalue[0]=(const char*)(&p1);
    PGresult *doublelist=PQexecPrepared(prbgdb, "objectdoublevalues", 1, parametervalue, length, pformat, 1 );

    if (doublelist != 0) {
        int count=PQntuples(doublelist);

        for(int i=0;i<count;i++) {
            uint64_t    valueid     = be64toh(*((uint64_t*)(PQgetvalue(doublelist, i, 0))));
            uint64_t valueindex  = be64toh(*((uint64_t*)(PQgetvalue(doublelist, i, 1))));
            uint64_t     dblvalueint = be64toh(*((uint64_t*)(PQgetvalue(doublelist,i, 2))));
            double       dblvalue;
            memcpy(&dblvalue, &dblvalueint, sizeof(double));

            obj->setvalue(obj, valueid, valueindex, dblvalue);
        }
    } else {
        Log.Error("Query of objectids failed: %s", PQerrorMessage(prbgdb));
    }
    PQclear(doublelist);

    PGresult *textlist=PQexecPrepared(prbgdb, "objecttextvalues", 1, parametervalue, length, pformat, 1 );

    if (textlist != 0) {
        int count=PQntuples(textlist);

        for(int i=0;i<count;i++) {
            uint64_t    valueid    = be64toh(*((uint64_t*)(PQgetvalue(textlist, i, 0))));
            uint64_t valueindex = be64toh(*((uint64_t*)(PQgetvalue(textlist, i, 1))));

            obj->setvalue(obj, valueid, valueindex, PQgetvalue(textlist,i, 2));
        }
    } else {
        Log.Error("Query of objectids failed: %s", PQerrorMessage(prbgdb));
    }
    PQclear(textlist);

    PGresult *integerlist=PQexecPrepared(prbgdb, "objectintegervalues", 1, parametervalue, length, pformat, 1 );

    if (integerlist != 0) {
        int count=PQntuples(integerlist);
        for(int i=0;i<count;i++) {
            uint64_t    valueid    = be64toh(*((uint64_t*)(PQgetvalue(integerlist, i, 0))));
            uint64_t valueindex = be64toh(*((uint64_t*)(PQgetvalue(integerlist, i, 1))));
            uint64_t     value      = be64toh(*((uint64_t*)(PQgetvalue(integerlist, i, 2))));

            obj->setvalue(obj, valueid, valueindex, value);

        }
    } else {
        Log.Error("Query of objectids failed: %s", PQerrorMessage(prbgdb));
    }
    PQclear(integerlist);


    PGresult *reflist=PQexecPrepared(prbgdb, "objectreferences", 1, parametervalue, length, pformat, 1 );

    if (reflist != 0) {
        int count=PQntuples(reflist);
        for(int i=0;i<count;i++) {
            uint64_t    valueid    = be64toh(*((uint64_t*)(PQgetvalue(reflist, i, 0))));
            uint64_t valueindex = be64toh(*((uint64_t*)(PQgetvalue(reflist, i, 1))));
            uint64_t   otherid    = be64toh(*((uint64_t*)(PQgetvalue(reflist, i, 2))));

            obj->setvalue(obj, valueid, valueindex, otherid);

        }
    } else {
        Log.Error("Query of objectids failed: %s", PQerrorMessage(prbgdb));
    }
    PQclear(reflist);
    return (0);
}

#endif

PGresult* tPG::db_createobject(uint64_t objectid, uint64_t type, uint64_t droptime) {
    const char *parametervalue[3];
    int        length[3] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1, 1, 1};
    uint64_t   p1        = htobe64(objectid);
    uint64_t   p2        = htobe64(type);
    uint64_t   p3        = htobe64(droptime);

    parametervalue[0]    = (const char*)(&p1);
    parametervalue[1]    = (const char*)(&p2);
    parametervalue[2]    = (const char*)(&p3);

    PGresult *docreate=PQexecPrepared(db, "createobject", 3, parametervalue, length, pformat, 1 );

    if (docreate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Creation of object failed %s", PQerrorMessage(db));
    }
    return (docreate);
}

PGresult* tPG::db_createfromtemplate(uint64_t templateid, uint64_t targettypeid, uint64_t targetid) {
    const char*  parametervalue[3];
    int          length[3] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int          pformat[] = {1,1,1};
    uint64_t     p1        = htobe64(templateid);
    uint64_t     p2        = htobe64(targettypeid);
    uint64_t     p3        = htobe64(targetid);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);

    PGresult *docreate=PQexecPrepared(db, "createfromtemplate", 3, parametervalue, length, pformat, 1 );

    if (docreate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Creation from template failed %s", PQerrorMessage(db));
    }
    return (docreate);
}

PGresult* tPG::db_deleteobject(uint64_t objectid) {
    const char *parametervalue[1];
    int        length[1] = {sizeof(uint64_t)};
    int        pformat[] = {1};
    uint64_t   p1        = htobe64(objectid);

    parametervalue[0] = (const char*)(&p1);

    PGresult *docreate=PQexecPrepared(db, "deleteobject", 1, parametervalue, length, pformat, 1 );

    if (docreate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Delete of object failed %s", PQerrorMessage(db));
    }
    return (docreate);
}

PGresult* tPG::db_createdoublevalue(uint64_t objid, uint64_t valueid, uint64_t index, double value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(double)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);
    uint64_t   p4        = htobe64(*((uint64_t*)(&value)));

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(&p4);

    PGresult *doupdate=PQexecPrepared(db, "createdoublevalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error,  "Create of a double value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %lf\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_updatedoublevalue(uint64_t objid, uint64_t valueid, uint64_t index, double value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(double)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);
    uint64_t   p4        = htobe64(*((uint64_t*)(&value)));

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(&p4);

    PGresult *doupdate=PQexecPrepared(db, "updatedoublevalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error,  "Update of a double value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %lf\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_deletedoublevalue(uint64_t objid, uint64_t valueid, uint64_t index) {
    const char *parametervalue[3];
    int        length[3] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);

    PGresult *dodelete=PQexecPrepared(db, "deletedoublevalue", 3, parametervalue, length, pformat, 1 );

    if (dodelete == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Delete of a double value failed. %s. Object: %lu Value-ID: %lu Index: %lu\n",
                PQerrorMessage(db), objid, valueid, index);
    }
    return (dodelete);
}

PGresult* tPG::db_createintvalue(uint64_t objid, uint64_t valueid, uint64_t index, int64_t value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(int64_t)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);
    uint64_t   p4        = htobe64(value);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(&p4);

    PGresult *doupdate=PQexecPrepared(db, "createintvalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Create of a double value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %li\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_updateintvalue(uint64_t objid, uint64_t valueid, uint64_t index, int64_t value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(int64_t)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);
    uint64_t   p4        = htobe64(value);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(&p4);

    PGresult *doupdate=PQexecPrepared(db, "updateintvalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Update of a integer value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %li\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_deleteintvalue(uint64_t objid, uint64_t valueid, uint64_t index) {
    const char *parametervalue[3];
    int        length[3] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);

    parametervalue[0]    = (const char*)(&p1);
    parametervalue[1]    = (const char*)(&p2);
    parametervalue[2]    = (const char*)(&p3);

    PGresult *dodelete=PQexecPrepared(db, "deleteintvalue", 3, parametervalue, length, pformat, 1 );

    if (dodelete == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Delete of a integer value failed. %s. Object: %lu Value-ID: %lu Index: %lu\n",
                PQerrorMessage(db), objid, valueid, index);
    }
    return (dodelete);
}

PGresult* tPG::db_createstringvalue(uint64_t objid, uint64_t valueid, uint64_t index, const char* value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), (int)strlen(value)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(value);

    PGresult *doupdate=PQexecPrepared(db, "createstringvalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Create of a string value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %s\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_updatestringvalue(uint64_t objid, uint64_t valueid, uint64_t index, const char* value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), (int)strlen(value)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(value);

    PGresult *doupdate=PQexecPrepared(db, "updatestringvalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Update of a string value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %s\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_deletestringvalue(uint64_t objid, uint64_t valueid, uint64_t index) {
    const char *parametervalue[3];
    int        length[3] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);

    PGresult *dodelete=PQexecPrepared(db, "deletestringvalue", 3, parametervalue, length, pformat, 1 );

    if (dodelete == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Delete of a string value failed. %s. Object: %lu Value-ID: %lu Index: %lu\n",
                PQerrorMessage(db), objid, valueid, index);
    }
    return (dodelete);
}

PGresult* tPG::db_createreferencevalue(uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);
    uint64_t   p4        = htobe64(value);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(&p4);

    PGresult *doupdate=PQexecPrepared(db, "createreferencevalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Create of a reference value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %lu\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_updatereferencevalue(uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);
    uint64_t   p4        = htobe64(value);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(&p4);

    PGresult *doupdate=PQexecPrepared(db, "updatereferencevalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "Update of a reference value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %lu\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_deletereferencevalue(uint64_t objid, uint64_t valueid, uint64_t index) {
    const char *parametervalue[3];
    int        length[3] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);

    PGresult *dodelete=PQexecPrepared(db, "deletereferencevalue", 3, parametervalue, length, pformat, 1 );

    if (dodelete == nullptr) {
        pgsqllogger.Write(LogLevel::Error,  "Delete of a reference value failed. %s. Object: %lu Value-ID: %lu Index: %lu\n",
                PQerrorMessage(db), objid, valueid, index);
    }
    return (dodelete);
}

PGresult* tPG::db_setintvalue(uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);
    uint64_t   p4        = htobe64(value);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(&p4);

    PGresult *doupdate=PQexecPrepared(db, "setintvalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "set-value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %lu\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_setrefvalue(uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);
    uint64_t   p4        = htobe64(value);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(&p4);

    PGresult *doupdate=PQexecPrepared(db, "setreferencevalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "set-value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %lu\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_settextvalue(uint64_t objid, uint64_t valueid, uint64_t index, const char* value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), (int)strlen(value)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(value);

    PGresult *doupdate=PQexecPrepared(db, "setstringvalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error, "set-value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %s\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

PGresult* tPG::db_setdblvalue(uint64_t objid, uint64_t valueid, uint64_t index, const double value) {
    const char *parametervalue[4];
    int        length[4] = {sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(uint64_t)};
    int        pformat[] = {1,1,1,1};
    uint64_t   p1        = htobe64(objid);
    uint64_t   p2        = htobe64(valueid);
    uint64_t   p3        = htobe64(index);
    uint64_t   p4        = htobe64(*((uint64_t*)(&value)));

    parametervalue[0] = (const char*)(&p1);
    parametervalue[1] = (const char*)(&p2);
    parametervalue[2] = (const char*)(&p3);
    parametervalue[3] = (const char*)(&p4);

    PGresult *doupdate=PQexecPrepared(db, "setdoublevalue", 4, parametervalue, length, pformat, 1 );

    if (doupdate == nullptr) {
        pgsqllogger.Write(LogLevel::Error,  "set-value failed. %s. Object: %lu Value-ID: %lu Index: %lu Value: %lf\n",
                PQerrorMessage(db), objid, valueid, index, value);
    }
    return (doupdate);
}

tSQLRecordSet* tPG::db_sqlfetch(uint32_t limit) {
    tSQLRecordSet*    result   = new tSQLRecordSet;
    uint32_t          fieldcount;
    uint64_t          endposition;
    std::vector <Oid> fieldtype;

    result->Count      = std::min ((size_t)(limit), (PQntuples(queryresult) - position));
    result->FieldCount = PQnfields(queryresult);
    //
    // Retrieve the fieldnames and types.
    for (fieldcount = 0; fieldcount < result->FieldCount; fieldcount++) {
        result->FieldNames.push_back(PQfname(queryresult, fieldcount));
        fieldtype.push_back(PQftype(queryresult, fieldcount));
    }

    result->Records.reserve(result->Count);
    endposition = position + result->Count;

    for (; position < endposition; position++) {
        tRecord           r;

        r.Number = position;
        r.Fields.reserve(result->FieldCount);

        for (fieldcount=0; fieldcount < result->FieldCount; fieldcount++) {
            tVariant field = PQgetvalue(queryresult, position, fieldcount);

            r.Fields.push_back(field);
        }
        result->Records.push_back(r);
    }

    return result;
}


tSQLRecordSet* tPG::db_sqlexecstring(const char *cmd, uint32_t limit) {
    int            err      = 0;
    tSQLRecordSet* result   = new tSQLRecordSet;
    //
    //  Clear old query result.
    if (queryresult != nullptr) {
        PQclear(queryresult);
    }
    queryresult = PQexec(db, cmd);
    if (queryresult != nullptr) {

        //
        //  First check for some execution error.
        ExecStatusType resultstat = PQresultStatus(queryresult);

        if (resultstat == PGRES_FATAL_ERROR) {
            //
            //  Now check the connection.
            ConnStatusType connstat = PQstatus(db);
            //
            //  Put back the last action
            if (connstat != CONNECTION_OK) {
                err = -1;
                WaitForReconnect();
            }
        }



        uint32_t          fieldcount;
        std::vector <Oid> fieldtype;
        //
        //  Init the RecordSet
        if (err == 0) {
            position     = 0;
            result->Count      = std::min ((size_t)(limit), (PQntuples(queryresult) - position));
            result->FieldCount = PQnfields(queryresult);
            //
            // Retrieve the fieldnames and types.
            for (fieldcount = 0; fieldcount < result->FieldCount; fieldcount++) {
                result->FieldNames.push_back(PQfname(queryresult, fieldcount));
                fieldtype.push_back(PQftype(queryresult, fieldcount));
            }

            result->Records.reserve(result->Count);

            for (; position < result->Count; position++) {
                tRecord           r;

                r.Number = position;
                r.Fields.reserve(result->FieldCount);

                for (fieldcount=0; fieldcount < result->FieldCount; fieldcount++) {
                    tVariant field = PQgetvalue(queryresult, position, fieldcount);

                    r.Fields.push_back(field);
                }
                result->Records.push_back(r);
            }
        }
    } else {
        err = -1;
        WaitForReconnect();
    }
    result->Error = err;
    return (result);
}
