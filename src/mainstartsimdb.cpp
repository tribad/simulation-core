#include <msg.h>
#include <CEvent.h>
#include <msgqueue.h>
#include <sim.h>
#include <mainctrl.h>
#include <mainstartsimdb.h>

#include <db.h>

bool CMainStartSimDB::Action() {
    bool retval = true;

    switch (substate) {
    case 0:
        stdb::open(mainthread, dbname, dbhost, dbport, dbuser, dbpasswd);
        break;
    case 1:
         stdb::preparestorage(mainthread, core.simdbhandle, history, versions, objectsondemand);
        break;
    case 2:
        break;
    default:
        break;
    }
    return retval;
}

CMainCtrl::State CMainStartSimDB::Process(tSigSQLConnectReplyPtr aMsg) {
    CMainCtrl::State retval = CMainCtrl::state;

    switch (substate) {
    case 0:
        if (aMsg->handle >= 0) {
            core.simdbhandle = aMsg->handle;
            coreIfc.stdbhandle = aMsg->handle;
            substate++;
        } else {
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

            retval = CMainCtrl::eRunning;
            substate = 2;
        }
        break;
    default:
        break;
    }

    return (retval);
}


CMainCtrl::State CMainStartSimDB::Process(tSigPrepareStorageReplyPtr aMsg) {
    (void)aMsg;
    CMainCtrl::State retval = CMainCtrl::state;

    switch (substate) {
    case 1:
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

        retval = CMainCtrl::eRunning;
        break;
    default:
        break;
    }
    substate++;

    return (retval);
}
