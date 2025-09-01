// MIT License
//
// Copyright (c) 2018 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#include <cstring>

#include "cacheentry.h"

#include "CSystemTime.h"
#include "netpack.h"
#include "httpresponse.h"

tHttpResponse::tHttpResponse()
{
    CSystemTime now;
    headerfields.push_back(std::pair<std::string, std::string>("Server", "Simulated-Universe HTTP Backend"));
    headerfields.push_back(std::pair<std::string, std::string>("Date", now.Get1123Date()));
}

void tHttpResponse::addheader(const std::string& name, const std::string& value) {
    headerfields.push_back(std::pair<std::string, std::string>(name, value));
}

const char* tHttpResponse::GetResponseText(uint32_t code) {
    //
    //
    (void)code;
    //
    const char* responsetext;

    switch (responsecode) {
    case 100:
        responsetext = "Continue";
        break;
    case 101:
        responsetext = "Switching Protocols";
        break;
    case 200:
        responsetext = "OK";
        break;
    case 201:
        responsetext = "Created";
        break;
    case 202:
        responsetext = "Accepted";
        break;
    case 203:
        responsetext = "Non-Authoritative Information";
        break;
    case 204:
        responsetext = "No Content";
        break;
    case 205:
        responsetext = "Reset Content" ;
        break;
    case 206:
        responsetext = "Partial Content";
        break;
    case 300:
        responsetext = "Multiple Choice";
        break;
    case 301:
        responsetext = "Moved Permanently";
        break;
    case 302:
        responsetext = "Found";
        break;
    case 303:
        responsetext = "See Other";
        break;
    case 304:
        responsetext = "Not Modified";
        break;
    case 305:
        responsetext = "Use Proxy";
        break;
    case 307:
        responsetext = "Temporary Redirect";
        break;
    case 308:
        responsetext = "Permanent Redirect";
        break;
    case 400:
        responsetext = "Bad Request";
        break;
    case 401:
        responsetext = "Unauthorized";
        break;
    case 403:
        responsetext = "Forbidden";
        break;
    case 404:
        responsetext = "Not Found";
        break;
    case 405:
        responsetext = "Method Not Allowed";
        break;
    case 406:
        responsetext = "Not Acceptable";
        break;
    case 407:
        responsetext = "Proxy Authentication Required";
        break;
    case 408:
        responsetext = "Request Timeout";
        break;
    case 409:
        responsetext = "Conflict";
        break;
    case 410:
        responsetext = "Gone";
        break;
    case 411:
        responsetext = "Length Required";
        break;
    case 412:
        responsetext = "Precondition Failed";
        break;
    case 413:
        responsetext = "Requested Entity Too Large";
        break;
    case 414:
        responsetext = "URI Too Long";
        break;
    case 415:
        responsetext = "Unsupported Media Type";
        break;
    case 416:
        responsetext = "Requested range not satisfiable";
        break;
    case 417:
        responsetext = "Expection Failed";
        break;
    case 418:
        responsetext = "";
        break;
    case 426:
        responsetext = "Upgrade Required";
        break;
    case 428:
        responsetext = "Precondition Required";
        break;
    case 429:
        responsetext = "Too Many Requests";
        break;
    case 431:
        responsetext = "Request Header Fields Too Large";
        break;
    case 451:
        responsetext = "Unavailable For Legal Reasons";
        break;
    case 500:
        responsetext = "Internal Server Error";
        break;
    case 501:
        responsetext = "Not Implemented";
        break;
    case 502:
        responsetext = "Bad Gateway";
        break;
    case 503:
        responsetext = "Service Unavailable";
        break;
    case 504:
        responsetext = "Gateway Timeout";
        break;
    case 505:
        responsetext = "HTTP Version not supported";
        break;
    case 511:
        responsetext = "Network Authentication Required";
        break;
    default:
        responsetext = "Internal Server Error";
        break;
    }
    return responsetext;
}

void tHttpResponse::fill(tNetPack *packet, CFile& file) {
    char* d=(char*)(packet->buffer);
    //
    //  Fill the response status line
    d+=snprintf(d, packet->size, "HTTP/1.1 %i %s\x0d\x0a", responsecode, GetResponseText(responsecode));
    //
    //  Fill the headers.
    std::list<std::pair<std::string, std::string> >::iterator hi;
    for (hi=headerfields.begin(); hi != headerfields.end(); ++hi) {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "%s: %s\x0d\x0a", hi->first.c_str(), hi->second.c_str());
    }
    if (contenttype != 0) {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Type: %s\x0d\x0a", contenttype);
    } else {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Type: text/html\x0d\x0a");
    }

    size_t filesize = file.GetInfo().GetSize();
    d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Length: %lu\x0d\x0a\x0d\x0a", filesize);

    if (file.Open() == 0) {
        file.Read(d, filesize);
        d+=filesize;
        file.Close();
    }

    packet->len = (((uint8_t*)(d))-packet->buffer);
}
void tHttpResponse::fill(tNetPack *packet, std::shared_ptr<CCacheEntry> entry) {
    char* d=(char*)(packet->buffer);
    //
    //  Fill the response status line
    d+=snprintf(d, packet->size, "HTTP/1.1 %i %s\x0d\x0a", responsecode, GetResponseText(responsecode));
    //
    //  Fill the headers.
    std::list<std::pair<std::string, std::string> >::iterator hi;
    for (hi=headerfields.begin(); hi != headerfields.end(); ++hi) {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "%s: %s\x0d\x0a", hi->first.c_str(), hi->second.c_str());
    }
    if (contenttype != 0) {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Type: %s\x0d\x0a", contenttype);
    } else {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Type: text/html\x0d\x0a");
    }

    size_t filesize = entry->Info.GetSize();
    d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Length: %lu\x0d\x0a\x0d\x0a", filesize);

    memcpy(d, entry->Buffer, filesize);

    d+=filesize;

    packet->len = (((uint8_t*)(d))-packet->buffer);
}
void tHttpResponse::dummyfill(tNetPack *packet, size_t size) {
    char* d=(char*)(packet->buffer);
    //
    //  Fill the response status line
    d+=snprintf(d, packet->size, "HTTP/1.1 %i %s\x0d\x0a", responsecode, GetResponseText(responsecode));
    //
    //  Fill the headers.
    std::list<std::pair<std::string, std::string> >::iterator hi;
    for (hi=headerfields.begin(); hi != headerfields.end(); ++hi) {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "%s: %s\x0d\x0a", hi->first.c_str(), hi->second.c_str());
    }
    if (contenttype != 0) {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Type: %s\x0d\x0a", contenttype);
    } else {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Type: text/html\x0d\x0a");
    }

    d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Length: %lu\x0d\x0a\x0d\x0a", size);

    packet->len = (((uint8_t*)(d))-packet->buffer);

}

void tHttpResponse::fill(tNetPack *packet) {
    char* d=(char*)(packet->buffer);
    //
    //  Fill the response status line
    d+=snprintf(d, packet->size, "HTTP/1.1 %i %s\x0d\x0a", responsecode, GetResponseText(responsecode));
    //
    //  Fill the headers.
    std::list<std::pair<std::string, std::string> >::iterator hi;
    for (hi=headerfields.begin(); hi != headerfields.end(); ++hi) {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "%s: %s\x0d\x0a", hi->first.c_str(), hi->second.c_str());
    }
    if (contenttype != 0) {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Type: %s\x0d\x0a", contenttype);
    } else {
        d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Type: text/html\x0d\x0a");
    }
    d+=snprintf(d, (packet->size - (((uint8_t*)(d))-packet->buffer)),  "Content-Length: %lu\x0d\x0a\x0d\x0a", content.str().size());

    memcpy(d, content.str().c_str(), content.str().size());
    d+=content.str().size();
    packet->len = (((uint8_t*)(d))-packet->buffer);

}

void tHttpResponse::setcookie(const std::string& aName, const std::string& aValue) {
    std::string cookie;

    cookie = aName+"="+aValue;
    addheader("Set-Cookie", cookie);
}

t404Response::t404Response() {
    responsecode = 404;
}

t303Response::t303Response() {
    responsecode = 303;
}

t303Response::t303Response(const std::string & aTarget) {
    responsecode = 303;
    addheader("Location", aTarget);
}

t307Response::t307Response() {
    responsecode = 307;
}

t307Response::t307Response(const std::string & aTarget) {
    responsecode = 307;
    addheader("Location", aTarget);
}
t308Response::t308Response() {
    responsecode = 308;
}

t308Response::t308Response(const std::string & aTarget) {
    responsecode = 308;
    addheader("Location", aTarget);
}

tOKResponse::tOKResponse() {
    responsecode = 200;
}

tInternalServerErrorResponse::tInternalServerErrorResponse() {
    responsecode = 500;
}
