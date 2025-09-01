#include <iomanip>
#include <sstream>
#include <memory.h>
#include "none-json-proto.h"
#include "logger.h"
#include "commthread.h"

extern long loglevel;

CNoneJSONProtocol::CNoneJSONProtocol(CLogger &aLogger) : CProtocol(aLogger) {
    json = 0;
}

std::list<std::shared_ptr<tNetPack> > CNoneJSONProtocol::Handle(std::shared_ptr<tNetPack> aPacket) {
    std::list<std::shared_ptr<tNetPack> > retval;

    if (loglevel & 0x1000) {
        char buffer[aPacket->len+1];

        memcpy(buffer, aPacket->buffer, aPacket->len);
        buffer[aPacket->len] = '\0';

        std::ostringstream oss;

        oss << "TCP-In  :" << std::hex << std::setw(16) << std::setfill('0') << aPacket->connid << "::" << std::dec << aPacket->len << "::" << buffer;
        Log.Write(LogLevel::Trace, oss);
    }
    /*
     * Parse the packet content. We expect JSON formated messages here.
     */
    json=parse(json, aPacket->buffer, aPacket->len);
    if (completed(json)) {
        CCommThread::instance().Put(aPacket->connid, json);

        json = 0;
    } else {
    }
    /*
     * We do get new packets all the time.
     */
    return retval;
}

