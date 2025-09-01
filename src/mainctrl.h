#pragma once
#ifndef MAINCTRL_H
#define MAINCTRL_H

#include <memory>

#include <tSigSQLConnectReply.h>
#include <tSigSQLExecStringReply.h>
#include <tSigSQLFetchReply.h>
#include <tSigPrepareStorageReply.h>


class CMainCtrl
{
public:
    enum State {
        eStartStorageDB,
        eLoadSimulation,
        eStartSimDB,
        eRunning,
        eInvalid = -1
    };
public:
    CMainCtrl() {}
    ~CMainCtrl() {}
    virtual bool Action() {return true;}
    virtual CMainCtrl::State Process(tSigSQLConnectReplyPtr aMsg) {(void)aMsg; return eInvalid;}
    virtual CMainCtrl::State Process(tSigSQLExecStringReplyPtr aMsg) {(void)aMsg; return eInvalid;}
    virtual CMainCtrl::State Process(tSigSQLFetchReplyPtr aMsg) {(void)aMsg; return eInvalid;}
    virtual CMainCtrl::State Process(tSigPrepareStorageReplyPtr aMsg) {(void)aMsg; return eInvalid;}
public:
    static CMainCtrl::State state;
    static CMainCtrl*    states[];
    //
    //  DB specifics
    static tCommTarget   mainthread;
    static std::string   dbtype;
    static std::string   dbname;
    static std::string   dbhost;
    static std::string   dbport;
    static std::string   dbuser;
    static std::string   dbpasswd;
    static int           maindbhandle;
    //
    //  The simulation
    //
    //  The storage
    static uint32_t      dbquerylimit;
    static bool          history;
    static bool          versions;
    static bool          objectsondemand;
public:
    static bool DoAction();
    static void Process(std::shared_ptr<tMsg> aMsg);
};

#endif // MAINCTRL_H
