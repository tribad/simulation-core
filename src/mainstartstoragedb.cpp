#include <msg.h>
#include <CEvent.h>
#include <msgqueue.h>
#include <sim.h>
#include <mainctrl.h>
#include <mainstartstoragedb.h>

#include <db.h>

#include "coreifc.h"

bool CMainStartStorageDB::Action() {
    bool retval = true;
    stdb::open(mainthread, dbname, dbhost, dbport, dbuser, dbpasswd);
    return retval;
}

CMainCtrl::State CMainStartStorageDB::Process(tSigSQLConnectReplyPtr aMsg) {
    coreIfc.stdbhandle = maindbhandle = aMsg->handle;

    if (coreIfc.stdbhandle >= 0) {
        return (CMainCtrl::eLoadSimulation);
    } else {
    }
    /*
 * This is an attribute variable that may later get its values from the DB.
 * But for now we take the standard values.
 * The simulation_id is the thread id of the simulation thread.
 */
    pthread_attr_t attr;
    pthread_t      simulation_id;
    /*
     * Start the thread now
     */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&simulation_id, &attr, simrun, 0);


    return CMainCtrl::eRunning;
}
