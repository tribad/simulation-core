// *************************************************************************************************************
//
//  Modul-Name     : tMsgInvalidReply.h
//
//  Copyrights by Hans-Juergen Lange <hjl@simulated-universe.de>. All rights reserved.
//
// *************************************************************************************************************
// HOC *********************************************************************************************************
//          |         |
// YYYYMMDD | Creator | description
//--------------------------------------------------------------------------------------------------------------
//          |         |
//----------+---------+-----------------------------------------------------------------------------------------
//          |         |
// HOC end *****************************************************************************************************
#pragma once
#ifndef TMSGINVALIDREPLY_INC
#define TMSGINVALIDREPLY_INC

#define IDM_INVALIDREPLY (0x6c3cc791349b97e1)

#include <string>
#include <msg.h>

struct tMsgInvalidReply : public tMsg {
    tMsgInvalidReply() {id = IDM_INVALIDREPLY;type = MSG_TYPE_REPLY;}
    tMsgInvalidReply(tJSON* j)  {(void)j; id = IDM_INVALIDREPLY;type = MSG_TYPE_REPLY;}
    virtual ~tMsgInvalidReply() {}
    virtual std::ostream& json(std::ostream& output) {
        output << "\"MsgId\": \"InvalidReply\"";
        output <<  ", \"MsgName\": \"" << this->MsgName << "\"";

        return output;
    }

    std::string MsgName;
};

using tMsgInvalidReplyPtr = std::shared_ptr<tMsgInvalidReply>;

#endif  // TMSGINVALIDREPLY_INC
