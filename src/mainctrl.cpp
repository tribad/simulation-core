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


