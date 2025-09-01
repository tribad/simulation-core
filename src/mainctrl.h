// MIT License
//
// Copyright (c) 2018 Hans-Juergen Lange<hjl@simulated-universe.de>
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
