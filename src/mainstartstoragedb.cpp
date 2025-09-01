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
