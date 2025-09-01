#ifndef NONE_JSON_PROTO_H
#define NONE_JSON_PROTO_H

#include "json.h"
#include "protocol.h"

class CNoneJSONProtocol : public CProtocol {
public:
    CNoneJSONProtocol(CLogger & aLogger) ;
    virtual ~CNoneJSONProtocol() = default;
    virtual std::list<std::shared_ptr<tNetPack> > Handle(std::shared_ptr<tNetPack> aPacket);
    tJSON*   json;
};

#endif
