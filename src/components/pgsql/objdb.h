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


#ifndef OBJDB_H
#define OBJDB_H

#include <map>
#include <list>
#include <libpq-fe.h>
#include <sqlrecordset.h>
#include <logger.h>

#include <dbmsg.h>

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
#include <tSigObjectLoadReq.h>
/*
 * This is an extract of the server side OIDs for builtin types.
 */
#define BOOLOID               16
#define BYTEAOID              17
#define CHAROID               18
#define NAMEOID               19
#define INT8OID               20
#define INT2OID               21
#define INT2VECTOROID         22
#define INT4OID               23
#define REGPROCOID            24
#define TEXTOID               25
#define OIDOID                26
#define TIDOID                27
#define XIDOID                28
#define CIDOID                29
#define OIDVECTOROID          30
#define JSONOID              114
#define XMLOID               142
#define PGNODETREEOID        194
#define POINTOID             600
#define LSEGOID              601
#define PATHOID              602
#define BOXOID               603
#define POLYGONOID           604
#define LINEOID              628
#define FLOAT4OID            700
#define FLOAT8OID            701
#define ABSTIMEOID           702
#define RELTIMEOID       	 703
#define TINTERVALOID     	 704
#define UNKNOWNOID           705
#define CIRCLEOID            718
#define CASHOID              790
#define MACADDROID           829
#define INETOID              869
#define CIDROID              650
#define INT2ARRAYOID        1005
#define INT4ARRAYOID        1007
#define TEXTARRAYOID        1009
#define OIDARRAYOID         1028
#define FLOAT4ARRAYOID      1021
#define ACLITEMOID          1033
#define CSTRINGARRAYOID     1263
#define BPCHAROID           1042
#define VARCHAROID      	1043
#define DATEOID     		1082
#define TIMEOID             1083
#define TIMESTAMPOID    	1114
#define TIMESTAMPTZOID      1184
#define INTERVALOID     	1186
#define TIMETZOID           1266
#define BITOID              1560
#define VARBITOID           1562
#define NUMERICOID          1700
#define REFCURSOROID        1790
#define REGPROCEDUREOID     2202
#define REGOPEROID      	2203
#define REGOPERATOROID      2204
#define REGCLASSOID         2205
#define REGTYPEOID          2206
#define REGTYPEARRAYOID     2211
#define UUIDOID             2950
#define LSNOID              3220
#define TSVECTOROID         3614
#define GTSVECTOROID        3642
#define TSQUERYOID          3615
#define REGCONFIGOID        3734
#define REGDICTIONARYOID    3769
#define JSONBOID            3802
#define INT4RANGEOID		3904
#define RECORDOID       	2249
#define RECORDARRAYOID      2287
#define CSTRINGOID          2275
#define ANYOID          	2276
#define ANYARRAYOID         2277
#define VOIDOID     		2278
#define TRIGGEROID      	2279
#define EVTTRIGGEROID		3838
#define LANGUAGE_HANDLEROID	2280
#define INTERNALOID         2281
#define OPAQUEOID           2282
#define ANYELEMENTOID       2283
#define ANYNONARRAYOID      2776
#define ANYENUMOID          3500
#define FDW_HANDLEROID      3115
#define ANYRANGEOID         3831


#define RECONNECT_TIMEOUT      5

enum class PGStatus : unsigned long {
    disconnected = 0u,
    connected,
    waitphase,
    count
};

struct tPreparedStatement {
    std::string cmd;
    unsigned    pcount;
    PGresult*   prep;
};
//
//  the control structure for a db statemachine.
class tPG {
public:
    tPG(CLogger& logger, std::string aName, std::string aHost="", std::string aPort="", std::string aUser="", std::string aPasswd="");
    ~tPG();
public:
    //
    //  Message based
    tMsg* dbprepstorage(tSigPrepareStorageReqPtr sig) ;
    tMsg* dbsqlexecstringreq(tSigSQLExecStringReqPtr sig) ;
    tMsg* dbsetvalue(tMsgDBSetPtr msg) ;
    tMsg* dbreferencevalue(tMsgDBValuePtr msg) ;
    tMsg* dbstringvalue(tMsgDBValuePtr msg) ;
    tMsg* dbintvalue(tMsgDBValuePtr msg) ;
    tMsg* dbdoublevalue(tMsgDBValuePtr msg) ;
    tMsg* dbcreatefromtemplate(tMsgDBCreateFromTemplatePtr msg) ;
    tMsg* dbcreateobj(tMsgDBCreateObjectPtr msg) ;
    tMsg* dbdelobj(tMsgDBDeleteObjectPtr msg) ;
    tMsg* dbsqlfetchreq(tSigSQLFetchReqPtr sig) ;
    tMsg* dbsqldisconnectreq(tSigSQLDisconnectReqPtr msg) ;
    tMsg* dbobjectloadreq(tSigObjectLoadReqPtr msg);
    //
    //
    bool IsReadyForReconnect();
    void DoReconnect();
    bool IsConnected() { return (state == PGStatus::connected);}
private:
    //
    //  internals
    void    db_disconnect();
    void    db_preparestorage() ;
    void    db_reset();

    PGresult* db_createobject(uint64_t objectid, uint64_t type, uint64_t droptime) ;
    PGresult* db_createfromtemplate(uint64_t templateid, uint64_t tagettypeid, uint64_t targetid);
    PGresult* db_deleteobject(uint64_t objectid) ;

    PGresult* db_createdoublevalue(uint64_t objid, uint64_t valueid, uint64_t index, double value);
    PGresult* db_updatedoublevalue(uint64_t objid, uint64_t valueid, uint64_t index, double value);
    PGresult* db_deletedoublevalue(uint64_t objid, uint64_t valueid, uint64_t index);

    PGresult* db_createintvalue(uint64_t objid, uint64_t valueid, uint64_t index, int64_t value);
    PGresult* db_updateintvalue(uint64_t objid, uint64_t valueid, uint64_t index, int64_t value);
    PGresult* db_deleteintvalue(uint64_t objid, uint64_t valueid, uint64_t index);

    PGresult* db_createstringvalue(uint64_t objid, uint64_t valueid, uint64_t index, const char* value);
    PGresult* db_updatestringvalue(uint64_t objid, uint64_t valueid, uint64_t index, const char* value);
    PGresult* db_deletestringvalue(uint64_t objid, uint64_t valueid, uint64_t index);

    PGresult* db_createreferencevalue(uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value);
    PGresult* db_updatereferencevalue(uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value);
    PGresult* db_deletereferencevalue(uint64_t objid, uint64_t valueid, uint64_t index);

    PGresult* db_setintvalue(uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value);
    PGresult* db_setrefvalue(uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) ;
    PGresult* db_settextvalue(uint64_t objid, uint64_t valueid, uint64_t index, const char* value) ;
    PGresult* db_setdblvalue(uint64_t objid, uint64_t valueid, uint64_t index, const double value) ;


    void db_loadobject(uint64_t oid);

    tSQLRecordSet* db_sqlexecstring(const char* cmd, uint32_t limit);
    tSQLRecordSet* db_sqlfetch(uint32_t limit) ;
private:
    void dbconnect(const std::string& dbname, const std::string& user, const std::string& password, const std::string& port, const std::string& host) ;
    bool ResCommandOK(const PGresult* res, const char* cmd) ;
    bool ResTuplesOK(const PGresult* res, const char* cmd) ;
    void WaitForReconnect();
public:
    PGStatus                                  state;
    std::string                               name;
    std::string                               host;
    std::string                               port;
    std::string                               user;
    std::string                               passwd;
    PGconn*                                   db;
    PGresult*                                 queryresult;
    uint64_t                                  position;
    std::map<std::string, tPreparedStatement> prepared;
    std::list<std::shared_ptr<tMsg> >         temp;       //  Holds the messages while waiting for reconnect.
    time_t                                    reconnect;  //  If this is 0 there is no reconnect requested.
    CLogger&                                  pgsqllogger;
    //
    //
};

#if 0
extern PGresult *objlist;
extern PGresult *doublelist;
extern PGresult *integerlist;
extern PGresult *textlist;
extern PGresult *referencelist;
#endif


#endif // OBJDB_H
