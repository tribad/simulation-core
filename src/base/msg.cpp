// **************************************************************************
//
//  Modul-Name     : msg.cpp
//  Author(s)      : Hans-Juergen Lange
//
//  Copyrights 2014 by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include "simobj.h"
#include "dbmsg.h"

std::atomic<uint64_t>   tMsg::gRequestSequence    = 0UL;
std::atomic<uint64_t>   tMsg::gIndicationSequence = 0UL;
std::atomic<uint64_t>   tMsg::gMsgCounter         = 0UL;

void freemsg(tMsg *msg) {
    switch (msg->id) {
    case eMsgDBValueString:
        if (((tMsgDBValue*)msg)->value.str!=0) {
            free(((tMsgDBValue*)msg)->value.str);
        }
        break;
    default:
        break;
    }
    free(msg);
}

