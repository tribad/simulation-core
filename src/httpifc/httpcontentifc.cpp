#include <memory>
#include "httpresponse.h"
#include "httpcontentifc.h"

CHttpContentIfc::CHttpContentIfc()
{
    RequestCount  = 0;
    ResponseCount = 0;
}

CHttpContentIfc::~CHttpContentIfc() {

}

bool CHttpContentIfc::DoYouHandleURI(const char *uri) {
    (void)uri;

    return false;
}

tHttpResponse* CHttpContentIfc::HandleURI(tHttpRequest *req) {
    (void)req;
    //
    return nullptr;
}

tHttpResponse* CHttpContentIfc::Process(tHttpRequest *req, std::shared_ptr<tMsg> msg) {
    //
    //
    (void)req;
    (void)msg;

    return new t404Response;
}
