#ifndef __HTTPREQUEST_INC__
#define __HTTPREQUEST_INC__

#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <map>

#define HTTP_REQUEST_LEN 16384

extern size_t HttpContentMax;

struct CompareCStrings
{
    bool operator()(const char* lhs, const char* rhs) const {
        return (strcmp(lhs, rhs) < 0);
    }
};
typedef  std::map<const char*, const char*, CompareCStrings>  charptrmap;

struct tHttpRequest {
    tHttpRequest(uint64_t aConnectionId) {
        content = 0;
        Init(aConnectionId);
    }
    void Init(uint64_t aConnectionId) {
        memset(buffer, 0, HTTP_REQUEST_LEN);
        client        = 0u;
        connid        = aConnectionId;
        method        = 0;
        uri           = 0;
        version       = 0;
        host          = 0;
        origin        = 0;
        key           = 0;
        proto         = 0;
        proto_version = 0;
        cookie        = 0;
        name          = 0;
        value         = 0;
        state         = 0;
        len           = 0;
        contenttype   = 0;
        contentlen    = 0;
        contentcount  = 0;
        if (content != 0) {
            delete [] content;
            content       = 0;
        }
        headerfields.clear();
        parameter.clear();
        c             = 0;
        b             = 0;
    }
    char     buffer[HTTP_REQUEST_LEN];
    uint32_t client;
    uint64_t connid;
    char*    method;
    char*    uri;
    char*    version;
    char*    host;
    char*    origin;
    char*    key;
    char*    proto;
    char*    proto_version;
    char*    cookie;
    char*    name;
    char*    value;
    int      state;
    int      len;
    char*    contenttype;
    int      contentlen;
    int      contentcount;
    char*    content;
    /*
     * Here we have some stuff to do URL-Encodeded Content processing.
     */
    char     c;    /* the character */
    char     b;    /* the bytecount */
    //const char* GetParameter(const char* name) {std::map<const char*, const char*>::iterator pi = parameter.find(name); if ()}
    charptrmap headerfields;
    charptrmap parameter;
};

#endif
