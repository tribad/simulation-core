// MIT License
//
// Copyright (c) 2016 Hans-Juergen Lange<hjl@simulated-universe.de>
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
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <sstream>
#include "netpack.h"
#include "connection.h"
#include "simifc/simifc.h"
#include "sim.h"
#include "logger.h"
#include "commthread.h"

#include "mainctrl.h"

#include "tMsgInvalidReply.h"

extern int      mainpid;
extern uint64_t loglevel;


CCommThread::CCommThread() : CSharedMsgThread("CommThread"), commlogger(ModulId::CommThread, "CommThread") {

}


void CCommThread::Process(std::shared_ptr<tMsg> aMsg) {
    tMsg* msgout;

    if (std:: holds_alternative<tReference>(aMsg->dst)) {
         msgout = core.simulation.ifc->Process(aMsg);
        if (msgout != nullptr) {
            q.Put(std::shared_ptr<tMsg>(msgout));
        } else {
        }
    } else if (std:: holds_alternative<tConnection>(aMsg->dst)) {
        auto conn = std::get<tConnection>(aMsg->dst);

        tMsgPackage* pack = new tMsgPackage(conn.id, aMsg);
        /*
         * Direct queing into the right threads signal queue.
         */
        int retval;
        int signal = ((pack->connid & SIGNAL_MASK) >> CONNECTIONID_BITS);
        union sigval sig;

        sig.sival_ptr = pack;
        retval = sigqueue(mainpid, signal, sig);
        if (retval != 0) {
            std::ostringstream oss;
            oss << "Cannot queue signal to " << signal;

            commlogger.Write(LogLevel::Critical, oss);
        }
    } else if (std:: holds_alternative<tDBQueue>(aMsg->dst)) {
    } else if (std:: holds_alternative<tQueue>(aMsg->dst)) {
        auto queue = std::get<tQueue>(aMsg->dst);
        //
        //  Checking that the queue is realy set.
        if (queue.q) {
            //
            //  To prevent endless loop for a message we check if it is pointing to ourselfs.
            if (queue.q != &q) {
                queue.q->Put(aMsg);
            }
        }

    } else if (std:: holds_alternative<tCoreQueue>(aMsg->dst)) {
        //  Maybe we need that some time.
        //auto queue = std::get<tCoreQueue>(aMsg->dst);

        std::get<tCoreQueue>(CMainCtrl::mainthread).q->Put(aMsg);
    } else {
        std::ostringstream oss;
        oss << "Drop message with unknown target type\n";

        commlogger.Write(LogLevel::Critical, oss);
    }
}

void CCommThread::Start() {
    instance().Create();

    while (!instance().Running) {}
}

void CCommThread::Put(uint64_t aConnId, tJSON *aJson) {
    //
    //  First check if there is a simulation that we can send messages to.
    //  As JSON only sends data into the simulation. At the moment not outgoing.
    if (core.simulation.ifc != nullptr) {
        if ((aJson != nullptr) && (loglevel & 0x800)) {
            std::ostringstream oss;
            dump(oss, aJson);
        }
        //
        //  Let the simulation create the message object.
        std::shared_ptr<tMsg> msgin = std::shared_ptr<tMsg>(core.simulation.ifc->GetMessage(aJson));

        if (msgin) {
            tConnection conn(aConnId, eCommFormat::JSON, msgin->seq);

            msgin->src = conn;
        } else {
            tMsgInvalidReply*   inv = new tMsgInvalidReply;
            tJSON*              id  = find(aJson, "MsgId");

            if (id != nullptr) {
                inv->MsgName = to_string(id);
            } else {
                id = find(aJson, "SignalId");

                if (id != nullptr) {
                    inv->MsgName = to_string(id);
                } else {
                    inv->MsgName = "<no message id set at all>";
                }
            }

            msgin = std::shared_ptr<tMsg>(inv);

            tConnection d (aConnId, eCommFormat::Raw);
            msgin->dst = d;
        }
        CCommThread::instance().q.Put(msgin);
    }
    //
    //
    if (aJson != nullptr) {
        //
        //  Because the root element of a json tree can only be a JSONObject we do a cast
        delete aJson;
    }
}


void CCommThread::Put(uint64_t aConnId, tMsg *aMsg) {
    //
    //  First check if there is a simulation that we can send messages to.
    //  As JSON only sends data into the simulation. At the moment not outgoing.
    if (core.simulation.ifc != nullptr) {
        //
        //  Let the simulation create the message object.
        std::shared_ptr<tMsg> msgin = std::shared_ptr<tMsg>(aMsg);

        tConnection s (aConnId, eCommFormat::Raw);

        msgin->src = s;

        if (msgin->dst.index() == 0) {
            tReference ref(0u, nullptr);

            msgin->dst = ref;
        }

        CCommThread::instance().q.Put(msgin);
    }
}

