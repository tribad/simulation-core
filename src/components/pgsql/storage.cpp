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

#include <list>
#include <map>
#include <algorithm>
#include <pthread.h>
#include <stdint.h>
#include <malloc.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <CEvent.h>
#include <CThread.h>
#include <msg.h>
#include <sharedmsgqueue.h>
#include <sharedmsgthread.h>
#include <logger.h>
#include <dbmsg.h>
#include <objdb.h>
//
//  PostgresDB connection object
//
//  Some more messages needed for special purpose.
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
#include <storage.h>

CSharedMsgQueue* OutQ;
//
//  Some local variables that are needed to run a seperat thread with
//  a message queue within.
uint64_t dbqlength   = 0;
//
//
static int dbcounter = 0;
//
//
tMsg* CStorageThread::dbsqlconnectreq(tSigSQLConnectReqPtr msg) {
    tSigSQLConnectReply* reply = new tSigSQLConnectReply;
    reply->dst = msg->src;
    reply->src = msg->dst;

    if (!msg->name.empty()) {
        tPG *pg = new tPG(pgsqllogger, msg->name, msg->host, msg->port, msg->user, msg->passwd);


        if (pg != nullptr) {
            pgconnection.insert(std::pair<int, tPG *>(++dbcounter, pg));
            reply->handle = dbcounter;
        } else {
            pgsqllogger.Write(LogLevel::Critical, "Could not create Connection-Object");
            reply->handle = -1;
        }
    } else {
        pgsqllogger.Write(LogLevel::Warn, "No DB name given. Working without.");
        reply->handle = -1;
    }
    return reply;
}
//
//  This is the message dispatcher in the storage thread.
void CStorageThread::Process(std::shared_ptr<tMsg> msg) {
    tMsg* ret = nullptr;
    //
    //  Should not happen but must be checked.
    if (msg != nullptr) {
        //
        //  Connecting to a DB is somewhat special.
        if (msg->id == IDS_SQLCONNECTREQ) {
            ret = dbsqlconnectreq(std::dynamic_pointer_cast<tSigSQLConnectReq>(msg));
        } else {
            //
            //  We should find a db connection.
            std::map<int, tPG*>::iterator pgi = pgconnection.find(((tMsgDB*)(msg.get()))->handle);
            //
            //  If we find the connection to the handle we start dispatching here.
            if (pgi != pgconnection.end()) {
                //
                //  Message dispatcher.
                switch (msg->id) {
                case eMsgDBTransactionStart:
                    break;
                case eMsgDBTransactionEnd:
                    break;
                case eMsgDBValueDouble:
                    ret = pgi->second->dbdoublevalue(std::static_pointer_cast<tMsgDBValue>(msg));
                    break;
                case eMsgDBValueU64:
                case eMsgDBValueI64:
                    ret = pgi->second->dbintvalue(std::static_pointer_cast<tMsgDBValue>(msg));
                    break;
                case eMsgDBValueString:
                    ret = pgi->second->dbstringvalue(std::static_pointer_cast<tMsgDBValue>(msg));
                    break;
                case eMsgDBValueReference:
                    ret = pgi->second->dbreferencevalue(std::static_pointer_cast<tMsgDBValue>(msg));
                    break;
                case eMsgDBDeleteObject:
                    ret = pgi->second->dbdelobj(std::static_pointer_cast<tMsgDBDeleteObject>(msg));
                    break;
                case eMsgDBCreateObject:
                    ret = pgi->second->dbcreateobj(std::static_pointer_cast<tMsgDBCreateObject>(msg));
                    break;
                case eMsgDBCreateFromTemplate:
                    ret = pgi->second->dbcreatefromtemplate(std::static_pointer_cast<tMsgDBCreateFromTemplate>(msg));
                    break;
                case eMsgDBSet:
                    ret =  pgi->second->dbsetvalue(std::static_pointer_cast<tMsgDBSet>(msg));
                    break;
                case IDS_SQLEXECSTRINGREQ:
                    //
                    //  This message get freed inside the call.
                    ret =  pgi->second->dbsqlexecstringreq(std::static_pointer_cast<tSigSQLExecStringReq>(msg));
                    break;
                case IDS_SQLDISCONNECTREQ:
                    //
                    //  We give an answer to the requester.
                    //  So we need not do anything here.
                    //  The msg get freed inside the call
                    ret = pgi->second->dbsqldisconnectreq(std::static_pointer_cast<tSigSQLDisconnectReq>(msg));

                    delete pgi->second;
                    pgconnection.erase(((tMsgDB*)(msg.get()))->handle);

                    break;
                case IDS_SQLFETCHREQ:
                    //
                    //  We give an answer to the requester.
                    //  So we need not do anything here.
                    //  The msg get freed inside the call
                    ret =  pgi->second->dbsqlfetchreq(std::static_pointer_cast<tSigSQLFetchReq>(msg));
                    break;
                case IDS_PREPARESTORAGEREQ:
                    ret =  pgi->second->dbprepstorage(std::static_pointer_cast<tSigPrepareStorageReq>(msg));
                    break;
                case IDS_OBJECTLOADREQ:
                    ret =  pgi->second->dbobjectloadreq(std::static_pointer_cast<tSigObjectLoadReq>(msg));
                    break;
                default:
                    //
                    //  Unknown message.
                    pgsqllogger.Write(LogLevel::Trace, "Got unknown message");
                    //
                    //  Dump it.
                    break;
                }
            } else {  // handle not found
                ret = new tMsgDBNAK();
                pgsqllogger.Write(LogLevel::Trace, "Got a request for an unknown handle. %i", ((tMsgDB*)msg.get())->handle);
            }
        } //  something different to a connect req.
    } else { //  message invalid.
        pgsqllogger.Write(LogLevel::Critical, "Message to process is <nullptr>.");
    }
    if (ret != nullptr) {
        OutQ->Put(std::shared_ptr<tMsg>(ret));
    }
}

void CStorageThread::ProcessWaitTimeout() {
    //
    //  Check for reconnecting connection objects.
    for (auto d : pgconnection) {
        if (d.second->IsReadyForReconnect()) {
            d.second->DoReconnect();
            //
            //  Check if reconnect succeeded
            if (d.second->IsConnected()) {
                //
                //  We make us a copy and replay all messages
                //  If we loose connection again they run back into the temp list.
                std::list<std::shared_ptr<tMsg> > mlist = d.second->temp;

                d.second->temp.clear();

                for (auto m : mlist) {
                    //
                    //  Do process the message
                    Process(m);
                }
            }
        }
    }
}

CStorageThread storage;

CStorageThread::CStorageThread() : CSharedMsgThread("PostgreSQL Storage Thread", 1000u) , pgsqllogger(ModulId::PostgreSQL, "PostgreSQLDB") {

}

//
//  Here are the function defined that are exported from the library.
//  There should be no undefined symbol within the lib.
extern "C" {
    //
    //  Init function for the storage thread.
    CSharedMsgQueue* init(CSharedMsgQueue* aOutQ) {
        //
        //  Setup the outgoing queue
        OutQ = aOutQ;
        //
        //  Create the storage-thread
        storage.Create();
        //
        //  Wait until the thread is up and running.
        while (!storage.Running) {}
        //
        //  Give a yell.
        storage.pgsqllogger.Write(LogLevel::Unbreakable, "Started up.");
        //
        //  Give the caller the queue for incoming messages.
        return &(storage.q);
    }
}
