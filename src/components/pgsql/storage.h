#pragma once
#ifndef SIMSTORAGE_H
#define SIMSTORAGE_H

#include <list>
#include "sharedmsgthread.h"

class CStorageThread : public CSharedMsgThread {
public:
    CStorageThread();
    virtual ~CStorageThread() {}
    virtual void Process(std::shared_ptr<tMsg> aMsg);
    void ProcessWaitTimeout(void) override;
private:
    tMsg* dbsqlconnectreq(tSigSQLConnectReqPtr msg) ;
public:
    CLogger             pgsqllogger;
    std::map<int, tPG*> pgconnection;
};

#endif // SIMSTORAGE_H
