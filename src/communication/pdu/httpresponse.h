#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <cstdint>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <memory>

#include "CFile.h"

#define HTTP_RESPONSE_LEN  16384

class tNetPack;
class CCacheEntry;

class tHttpResponse
{
public:
    tHttpResponse();
    void addheader(const std::string& name, const std::string& value);
    void fill(tNetPack* packet);
    void fill(tNetPack* packet, CFile& file);
    void fill(tNetPack* packet, std::shared_ptr<CCacheEntry> entry);
    void dummyfill(tNetPack* packet, size_t size);
    void setcookie(const std::string& aName, const std::string& aValue);
private:
    const char* GetResponseText(uint32_t code);
public:
    uint32_t                                            responsecode;
    const char*                                         contenttype;
    std::list< std::pair< std::string, std::string > >  headerfields;
    std::ostringstream                                  content;
};

class t404Response : public tHttpResponse
{
public:
    t404Response();
};

class t303Response : public tHttpResponse
{
public:
    t303Response();
    t303Response(const std::string& aTarget);
};

using tSeeOther = t303Response;

class t307Response : public tHttpResponse
{
public:
    t307Response();
    t307Response(const std::string& aTarget);
};

using tTemporaryRedirect = t307Response;

class t308Response : public tHttpResponse
{
public:
    t308Response();
    t308Response(const std::string& aTarget);
};

using tPermanentRedirect = t308Response;

class tOKResponse : public tHttpResponse
{
public:
    tOKResponse();
};

class tInternalServerErrorResponse : public tHttpResponse
{
public:
    tInternalServerErrorResponse();
};

#endif // HTTPRESPONSE_H
