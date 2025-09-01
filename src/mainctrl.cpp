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

#include <msg.h>
#include <tSigSQLConnectReply.h>
#include <tSigSQLExecStringReply.h>
#include <tSigSQLFetchReply.h>
#include <sim.h>
#include <mainctrl.h>
#include <mainstartstoragedb.h>
#include <mainrunning.h>
#include <mainloadsimulation.h>
#include <mainstartsimdb.h>

static CMainStartStorageDB StartStorage;
static CMainRunning        Running;
static CMainLoadSimulation LoadSimulation;
static CMainStartSimDB     StartSimDB;

CMainCtrl::State CMainCtrl::state        = eStartStorageDB;

CMainCtrl*       CMainCtrl::states[] = {&StartStorage, &LoadSimulation, &StartSimDB, &Running};
/*
 * The DB configuration info from configuration file.
 */
tCommTarget      CMainCtrl::mainthread;
std::string      CMainCtrl::dbname;
std::string      CMainCtrl::dbtype;
std::string      CMainCtrl::dbhost;
std::string      CMainCtrl::dbport;
std::string      CMainCtrl::dbuser;
std::string      CMainCtrl::dbpasswd;
int              CMainCtrl::maindbhandle = -1;
/*
 * This is the simulation core
 */

/*
 * This describes the "geometry" of the DB
 */
uint32_t         CMainCtrl::dbquerylimit    = 10000u;
bool             CMainCtrl::history         = false;
bool             CMainCtrl::versions        = false;
bool             CMainCtrl::objectsondemand = false;

bool CMainCtrl::DoAction() {
    bool retval = true;

    if (state != eInvalid)  {
        retval = states[state]->Action();
    }

    return retval;
}

void CMainCtrl::Process(std::shared_ptr<tMsg> aMsg) {
    if (state != eInvalid) {
        switch (aMsg->id) {
        case IDS_SQLCONNECTREPLY:
            state = states[state]->Process(std::static_pointer_cast<tSigSQLConnectReply>(aMsg));
            break;
        case IDS_SQLEXECSTRINGREPLY:
            state = states[state]->Process(std::static_pointer_cast<tSigSQLExecStringReply>(aMsg));
            break;
        case IDS_SQLFETCHREPLY:
            state = states[state]->Process(std::static_pointer_cast<tSigSQLFetchReply>(aMsg));
            break;
        case IDS_PREPARESTORAGEREPLY:
            state = states[state]->Process(std::static_pointer_cast<tSigPrepareStorageReply>(aMsg));
            break;
        default:
            state = CMainCtrl::State::eInvalid;
            break;
        }
    } else {

    }
}


