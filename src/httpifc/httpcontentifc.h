#ifndef HTTPCONTENTIFC_H
#define HTTPCONTENTIFC_H

#include <cstdint>
#include <helper.h>
#include <msg.h>
#include <logger.h>

class tHttpResponse;
class tHttpRequest;
class CHttpSimulationIfc;
class CFileCacheEntry;
class IFileCacheNotifier;

class CHttpContentIfc
{
public:
    CHttpContentIfc();
    virtual ~CHttpContentIfc();
    virtual bool DoYouHandleURI(const char* uri);
    virtual tHttpResponse* HandleURI(tHttpRequest* req);
    virtual tHttpResponse* Process(tHttpRequest* req, std::shared_ptr<tMsg> msg);
    bool ReadyToRelease() {return (RequestCount == ResponseCount);}
protected:
    uint64_t RequestCount;
    uint64_t ResponseCount;
};

typedef CHttpContentIfc* (*httpcontentifc_op)(xmlNode*, CHttpSimulationIfc*);

#endif // HTTPCONTENTIFC_H
