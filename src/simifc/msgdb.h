// **************************************************************************
//
// Modul-Name        : msgdb.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.01.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef TMSGDB_H
#define TMSGDB_H

#include <msg.h>

struct tMsgDB : public tMsg
{
    tMsgDB() {handle = -1;}
    virtual ~tMsgDB() = default;
    int handle;
};

#endif // TMSGDB_H
