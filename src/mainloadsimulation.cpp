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
#include <mainloadsimulation.h>

#include <db.h>

bool CMainLoadSimulation::Action() {
    bool retval = true;
    switch (substate) {
    case 0:
        //
        //  Request the number of columns that are occupied by the objects-table.
        //  This helps to indicate load-on-demand flags in the objects.
        stdb::sqlexecstring(mainthread, maindbhandle, 0x01, "select count(*) from information_schema.columns where table_name='objects';");
        break;
    default:
        break;
    }

    return retval;
}

CMainCtrl::State CMainLoadSimulation::Process(tSigSQLExecStringReplyPtr aMsg) {
    CMainCtrl::State         retval = CMainCtrl::eLoadSimulation;

    switch (substate) {
    case 0: //  checking if we have load-on-demand information in the objects.
        columncount = (*(aMsg->RecordSet))[0][0];
        if (columncount == 2) {
            objectsondemand = false;
        } else {
            objectsondemand = true;
        }
        stdb::sqlexecstring(mainthread, maindbhandle, 0x02, "select * from objects;", dbquerylimit);
        substate = 1;
        break;
    case 1:
        LoadObjects(aMsg->RecordSet);
        break;
    default:
        retval = LoadData(aMsg->RecordSet);
        break;
    }
    return retval;
}

CMainCtrl::State CMainLoadSimulation::Process(tSigSQLFetchReplyPtr aMsg) {
    CMainCtrl::State         retval = CMainCtrl::eLoadSimulation;

    switch (substate) {
    case 1:
        LoadObjects(aMsg->RecordSet);
        break;
    default:
        retval = LoadData(aMsg->RecordSet);
        break;
    }
    return retval;
}

void CMainLoadSimulation::LoadObjects(tSQLRecordSet *aData) {
    //
    //  If we got data we create the objects.
    //  If there are no more data from the DB we query the doublevalues.
    if (aData->Count != 0) {
        uint64_t droptimer = UINT64_MAX;

        for(size_t i=0;i<aData->Count;i++) {
            uint64_t oid = (*aData)[i][0];
            uint64_t tid = (*aData)[i][1];
            /*
             * If we have the ondemand column we read that too.
             */
            if (columncount > 2) {
                droptimer = (*aData)[i][2];
            }
            /*
             * Create object is done through the simulation viewport.
             */
            if (core.simulation.ifc != nullptr) {
                core.simulation.ifc->CreateObject(oid, tid, droptimer);
            }
        }
        stdb::sqlfetch(mainthread, maindbhandle, dbquerylimit);
    } else {
        if (objectsondemand) {
            stdb::sqlexecstring(mainthread, maindbhandle, 0x03, "select doublevalue.objectid, doublevalue.valueid, doublevalue.index, doublevalue.value from doublevalue join objects on doublevalue.objectid = objects.objectid where objects.droptime = -1;", dbquerylimit);
        } else {
            stdb::sqlexecstring(mainthread, maindbhandle, 0x03, "select * from doublevalue;", dbquerylimit);
        }
        substate = 2;
    }
}

CMainCtrl::State  CMainLoadSimulation::LoadData(tSQLRecordSet *aData) {
    CMainCtrl::State retval = CMainCtrl::eLoadSimulation;

    if (aData->Count != 0) {
        for(size_t i=0;i<aData->Count;i++) {
            uint64_t oid         = (*aData)[i][0];
            uint64_t valueid     = (*aData)[i][1];
            uint64_t valueindex  = (*aData)[i][2];
            /*
             * Setting the value into the object is done through the simulation viewport
             * if it exists.
             */
            if (core.simulation.ifc != 0) {
                core.simulation.ifc->SetValue(oid, valueid, valueindex, (*aData)[i][3]);
            } else {
            }
        }
        stdb::sqlfetch(mainthread, maindbhandle, dbquerylimit);
    } else {
        substate++;
        switch (substate) {
        case 3:
            if (objectsondemand) {
                stdb::sqlexecstring(mainthread, maindbhandle, 0x03, "select textvalue.objectid, textvalue.valueid, textvalue.index, textvalue.text from textvalue join objects on textvalue.objectid = objects.objectid where objects.droptime = -1;", dbquerylimit);
            } else {
                stdb::sqlexecstring(mainthread, maindbhandle, 0x03, "select * from textvalue;", dbquerylimit);
            }
            break;
        case 4:
            if (objectsondemand) {
                stdb::sqlexecstring(mainthread, maindbhandle, 0x03, "select integervalue.objectid, integervalue.valueid, integervalue.index, integervalue.value from integervalue join objects on integervalue.objectid = objects.objectid where objects.droptime = -1;", dbquerylimit);
            } else {
                stdb::sqlexecstring(mainthread, maindbhandle, 0x03, "select * from integervalue;", dbquerylimit);
            }
            break;
        case 5:
            if (objectsondemand) {
                stdb::sqlexecstring(mainthread, maindbhandle, 0x03, "select refs.objectid, refs.valueid, refs.index, refs.other from refs join objects on refs.objectid = objects.objectid where objects.droptime = -1;", dbquerylimit);
            } else {
                stdb::sqlexecstring(mainthread, maindbhandle, 0x03, "select * from refs;", dbquerylimit);
            }
            break;
        default:
            retval = CMainCtrl::eStartSimDB;
            break;
        }
    }
    return retval;
}
