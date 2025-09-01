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


#include <memory>

#include <memory.h>
#include <gnutls/gnutls.h>
#include <httpresponse.h>
#include <httprequest.h>
#include <httpcontentifc.h>
#include <httpdomainspec.h>
#include <http-proto.h>
#include <logger.h>
#include <commthread.h>
#include <connection.h>
#include <CFile.h>
#include <httpcontentlib.h>

#include <cachethread.h>

#define IS_NUMERIC_HEAD(value)   ((value & (0xdfdfdfdf)) == ('H'+'E'*256+'A'*65536+'D'*16777216))
#define IS_NUMERIC_POST(value)   ((value & (0xdfdfdfdf)) == ('P'+'O'*256+'S'*65536+'T'*16777216))
#define IS_NUMERIC_GET0(value)   ((value & (0xdfdfdfdf)) == ('G'+'E'*256+'T'*65536))

extern uint64_t loglevel;

std::mutex CHTTPProtocol::domains_lock;

std::map<std::string, CHttpDomainSpec*> CHTTPProtocol::domains;
std::map<std::string, std::string>      CHTTPProtocol::ext2mime;

CHTTPProtocol::CHTTPProtocol(uint64_t aConnId, CLogger &aLogger) : CProtocol(aLogger) {
    http         = new tHttpRequest(aConnId);
    PingCount    = 0;
    ConnectionId = aConnId;
    addtimer(FIRSTDATA_TIMEOUT, aConnId, (aConnId >> CONNECTIONID_BITS), FIRSTDATA_TIMERID);
}

CHTTPProtocol::~CHTTPProtocol() {
    if (http != 0) {
        delete http;
    }
}

const char* CHTTPProtocol::GetMimeType(const std::string& extension) {
    std::map<std::string, std::string>::iterator mi = CHTTPProtocol::ext2mime.find(extension);

    if (mi != CHTTPProtocol::ext2mime.end()) {
        return mi->second.c_str();
    }
    return "text/plain";
}

std::list<std::shared_ptr<tNetPack> > CHTTPProtocol::Handle(std::shared_ptr<tNetPack> aPacket) {
    std::list<std::shared_ptr<tNetPack> > retval;
    PingCount        = 0;
    /*
     * if the http-state is at end while receiving a new package we reset the http object.
     */
    if (http->state == -1) {
        http->Init(ConnectionId);
    }
    /*
     * The processing of the HTTP request is a state machine.
     * This way we can process the request step by step
     */
    ProcessHttp(aPacket);
    /*
     * If the http request has been completed we let the request run against
     * a library that implements dynamic page generation.
     */
    if (http->state == -1) {
        bool is_file = true;
        if (http->host != 0) {
            std::string host = http->host;
            if (host.find_first_of(':') != std::string::npos) {
                host = host.substr(0, host.find_first_of(':'));
            }

            std::map<std::string, CHttpDomainSpec*>::iterator di = domains.find(host);
            //
            //  Check if we handle this domain.
            if (di != domains.end()) {
                //
                //  To prevent content lib switches through different threads.
                di->second->ContentSwitchLock.lock();
                if ((di->second->mActual != nullptr) && (di->second->mActual->mContentIfc != nullptr)) {
                    if (di->second->mActual->mContentIfc->DoYouHandleURI(http->uri)) {
                        tHttpResponse* resp;
                        //
                        //  If the content library handles the uri it is not a file.
                        //  Anyway it may happen that the response returned is empty.
                        //  This may happen if the processing needs more data from the simulation.
                        is_file = false;

                        resp = di->second->mActual->mContentIfc->HandleURI(http);
                        if (resp != 0) {
                            std::shared_ptr<tNetPack> one;
                            one = std::make_shared<tNetPack>(HTTP_RESPONSE_LEN+resp->content.str().size());

                            resp->fill(one.get());
                            one->connid = ConnectionId;
                            retval.push_back(one);
                            delete resp;
                        }
                    } else {
                    }
                } else {

                }
                di->second->ContentSwitchLock.unlock();
                if (is_file) {
                    //
                    //  We expect the uri to be some sort of file
                    //  Only for / we may expand it to index.html as the default.
                    std::string localuri;

                    if (std::string(http->uri) == "/") {
                        localuri = "index.html";
                    } else {
                        localuri = http->uri;
                    }
                    //
                    //  After that we create the path to the file
                    //  and create a file object.
                    //CFile thefile(di->second->HtmlDir+"/"+localuri);
                    std::shared_ptr<CCacheEntry> thefile = CCacheThread::instance().Get(di->second->HtmlDir+"/"+localuri);
                    //
                    //  Check if the file exists.
                    if ((thefile != nullptr) && (thefile->Info.IsValid())) {
                        tOKResponse resp;
                        CFileName   filename(localuri);

                        resp.contenttype = GetMimeType(filename.Extension());
                        resp.addheader("Connection", "keep-alive");
                        resp.addheader("Cache-Control", "public, max-age="+di->second->MaxAge);

                        std::shared_ptr<tNetPack> one;
                        one = std::make_shared<tNetPack>(HTTP_RESPONSE_LEN+thefile->Info.GetSize());
                        //
                        //  Check if HEAD or GET.
                        if (IS_NUMERIC_HEAD(*(uint32_t*)(http->method))) {
                            resp.dummyfill(one.get(), thefile->Info.GetSize());
                        } else if ((IS_NUMERIC_GET0(*(uint32_t*)(http->method)) ) || (IS_NUMERIC_POST(*(uint32_t*)(http->method)) )) {
                            resp.fill(one.get(), thefile);
                        }
                        one->connid = ConnectionId;
                        retval.push_back(one);
                    } else {
                        //
                        //  Create a 404 response
                        t404Response resp;

                        resp.contenttype = "text/plain";
                        resp.content << "Not Found";
                        resp.addheader("Connection", "keep-alive");
                        std::shared_ptr<tNetPack> one;

                        one = std::make_shared<tNetPack>(HTTP_RESPONSE_LEN+resp.content.str().size());

                        resp.fill(one.get());
                        one->connid = ConnectionId;
                        retval.push_back(one);
                    }
                }
            }
        }
    }
    return retval;
}


std::shared_ptr<tNetPack> CHTTPProtocol::Process(std::shared_ptr<tMsg> aMsg) {
    std::shared_ptr<tNetPack> retval;

    if (aMsg) {
        if (http->host != nullptr) {
            std::string host = http->host;
            if (host.find_first_of(':') != std::string::npos) {
                host = host.substr(0, host.find_first_of(':'));
            }

            std::map<std::string, CHttpDomainSpec*>::iterator di = domains.find(host);
            //
            //  Check if we handle this domain.
            if (di != domains.end()) {
                tHttpResponse* resp;

                if ((di->second->mActual != nullptr) && (di->second->mActual->mContentIfc != nullptr)) {
                    resp = di->second->mActual->mContentIfc->Process(http, aMsg);
                    if (resp != nullptr) {
                        retval = std::make_shared<tNetPack>(HTTP_RESPONSE_LEN+resp->content.str().size());

                        resp->fill(retval.get());
                        delete resp;
                        retval->connid = ConnectionId;
                    }
                }
            }
        }
    }
    return retval;
}

std::shared_ptr<tNetPack> CHTTPProtocol::Process(std::shared_ptr<tNetPack> aPacket) {
    return aPacket;
}


int CHTTPProtocol::ProcessHttp(std::shared_ptr<tNetPack> aPacket) {
    int   error=0;
    char* s=(char*)(aPacket->buffer);

    do {
        /*
         * If we are copying the content area we must not do correction to the content.
         */
        if (http->state != 39) {
            /*
             * Check for HEX-formatted character.
             */
            switch (http->b) {
            case 0:
                if (*s == '%') {
                    http->b = 1;
                    http->c = 0;
                } else if (*s == '+') {
                    http->b = 0;
                    http->c = ' ';
                } else {
                    http->c = *s;
                }
                break;
            case 1:
                if (isxdigit(*s)) {
                    http->b = 2;
                    if (isdigit(*s)) {
                        http->c = ((*s)-'0') * 16;
                    } else {
                        http->c = (toupper(*s) -'A' + 10) * 16;
                    }
                } else {

                }
                break;
            case 2:
                if (isxdigit(*s)) {
                    http->b = 0;
                    if (isdigit(*s)) {
                        http->c += ((*s)-'0');
                    } else {
                        http->c += (toupper(*s) -'A' + 10);
                    }
                } else {

                }
                if (http->state == 44) {
                    http->contentlen -= 2;
                }
                break;
            default:
                break;
            }
        } else {
            http->c = *s;
        }
        if (http->b == 0) {
            http->buffer[http->len] = http->c;
            switch (http->state) {
            case 0:
                if ((http->c == 'G') || (http->c == 'g')) {
                    http->method=http->buffer;
                    http->len++;
                    http->state = 1;
                } else if ((http->c == 'P') || (http->c == 'p')) {
                    http->method=http->buffer;
                    http->len++;
                    http->state = 11;
                } else if ((http->c == 'H') || (http->c == 'h')) {
                    http->method=http->buffer;
                    http->len++;
                    http->state = 13;
                } else if ((http->c == 'D') || (http->c == 'd')) {
                    http->method=http->buffer;
                    http->len++;
                    http->state = 19;
                } else if ((http->c == 'T') || (http->c == 't')) {
                    http->method=http->buffer;
                    http->len++;
                    http->state = 24;
                } else if ((http->c == 'O') || (http->c == 'o')) {
                    http->method=http->buffer;
                    http->len++;
                    http->state = 27;
                } else if ((http->c == 'C') || (http->c == 'c')) {
                    http->method=http->buffer;
                    http->len++;
                    http->state = 33;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 1:
                if ((http->c == 'E') || (http->c == 'e')) {
                    http->len++;
                    http->state = 2;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 2:
                if ((http->c == 'T') || (http->c == 't')) {
                    http->len++;
                    http->state = 3;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 3:    //  wait until first non blank
                if (http->c != ' ') {
                    http->uri=http->buffer+http->len;
                    http->state = 4;
                } else {
                    http->buffer[http->len] = '\0';
                }
                http->len++;
                break;
            case 4:    // Wait until end of URI
                if (http->c == ' ') {
                    http->buffer[http->len] = '\0';
                    http->state = 5;
                } else if (http->c == '?') {
                    http->buffer[http->len] = '\0';
                    http->state = 40;
                }
                http->len++;
                break;
            case 5:    //  wait for next non blank
                if (http->c != ' ') {
                    http->version=http->buffer+http->len;
                    http->state = 6;
                } else {
                    http->buffer[http->len] = '\0';
                }
                http->len++;
                break;
            case 6:   //  Wait for end of line. 0x0d
                if (http->c != '\x0a') {
                    if ((http->c == ' ') || (http->c == '\x0d')) {
                        http->buffer[http->len] = '\0';
                    }
                } else {
                    http->buffer[http->len] = '\0';
                    http->state = 7;
                }
                http->len++;
                break;
            case 7:   //  Start readin a complete header line
                if (http->c == '\x0d') { //  complete.
                    http->state = 38;
                } else {
                    http->name = http->buffer+http->len;
                    http->state = 8;
                }
                http->len++;
                break;
            case 8:  //  wait colon, null space
                if (http->c == ' ') {
                    http->buffer[http->len] = '\0';
                } else {
                    if (http->c == ':') {
                        http->buffer[http->len] = '\0';
                        http->state = 9;
                    }
                }
                http->len++;
                break;
            case 9:     //  wait first non blank
                if (http->c != ' ') {
                    http->value = http->buffer+http->len;
                    http->state = 10;
                } else {
                    http->buffer[http->len] = '\0';
                }
                http->len++;
                break;
            case 10:   //  Wait for end of value
                if (http->c == '\x0d') {
                    http->buffer[http->len] = '\0';
                    if (strcasecmp(http->name, "host")==0) {
                        http->host = http->value;
                    } else if (strcasecmp(http->name, "origin")==0) {
                        http->origin = http->value;
                    } else if (strcasecmp(http->name, "Sec-WebSocket-Key")==0) {
                        http->key = http->value;
                    } else if (strcasecmp(http->name, "Sec-WebSocket-Protocol")==0) {
                        http->proto = http->value;
                    } else if (strcasecmp(http->name, "Sec-WebSocket-Version")==0) {
                        http->proto_version = http->value;
                    } else if (strcasecmp(http->name, "Content-Type")==0) {
                        http->contenttype = http->value;
                    } else if (strcasecmp(http->name, "Content-Length")==0) {
                        http->contentlen = strtoul(http->value, 0, 0);
                        if (http->contentlen > 0) {
                            http->content = (char*)malloc(http->contentlen+1);
                            //
                            //  This adds a 0-byte to the content whatever it is.
                            memset(http->content, 0, http->contentlen+1);
                        } else {
                        }
                    } else {

                    }
                    http->headerfields.insert(std::pair<const char*, const char*>(http->name, http->value));
                } else if (http->c == '\x0a') {
                    http->buffer[http->len] = '\0';
                    http->state = 7;
                }
                http->len++;
                break;
            case 11:    // pOst or pUt or pAtch
                if ((http->c == 'O') || (http->c == 'o')) {
                    http->len++;
                    http->state = 12;
                } else if ((http->c == 'U') || (http->c == 'u')) {
                    http->len++;
                    http->state = 2;
                } else if ((http->c == 'A') || (http->c == 'a')) {
                    http->len++;
                    http->state = 16;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 12:  // poSt
                if ((http->c == 'S') || (http->c == 's')) {
                    http->len++;
                    http->state = 2;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 13:  // hEad
                if ((http->c == 'E') || (http->c == 'e')) {
                    http->len++;
                    http->state = 14;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 14:  // heAd
                if ((http->c == 'A') || (http->c == 'a')) {
                    http->len++;
                    http->state = 15;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 15:  // heaD
                if ((http->c == 'D') || (http->c == 'd')) {
                    http->len++;
                    http->state = 3;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 16:  // paTch
                if ((http->c == 'T') || (http->c == 't')) {
                    http->len++;
                    http->state = 17;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 17:  // patCh
                if ((http->c == 'C') || (http->c == 'c')) {
                    http->len++;
                    http->state = 18;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 18:  // patcH
                if ((http->c=='H') || (http->c=='h')) {
                    http->len++;
                    http->state = 3;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 19:  // dElete
                if ((http->c=='E') || (http->c=='e')) {
                    http->len++;
                    http->state = 20;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 20:  // deLete
                if ((http->c=='L') || (http->c=='l')) {
                    http->len++;
                    http->state = 21;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 21:  // delEte
                if ((http->c=='E') || (http->c=='e')) {
                    http->len++;
                    http->state = 22;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 22:  // deleTe
                if ((http->c=='T') || (http->c=='T')) {
                    http->len++;
                    http->state = 23;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 23:  // deletE
                if ((http->c=='E') || (http->c=='e')) {
                    http->len++;
                    http->state = 3;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 24:  // tRace
                if ((http->c=='R') || (http->c=='r')) {
                    http->len++;
                    http->state = 25;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 25:  // trAce
                if ((http->c=='A') || (http->c=='a')) {
                    http->len++;
                    http->state = 26;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 26:  // traCe
                if ((http->c=='C') || (http->c=='c')) {
                    http->len++;
                    http->state = 3;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 27:  // oPtions
                if ((http->c=='P') || (http->c=='p')) {
                    http->len++;
                    http->state = 28;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 28:  // opTions
                if ((http->c=='T') || (http->c=='t')) {
                    http->len++;
                    http->state = 29;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 29:  // optIons
                if ((http->c=='I') || (http->c=='i')) {
                    http->len++;
                    http->state = 30;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 30:  // optiOns
                if ((http->c=='O') || (http->c=='o')) {
                    http->len++;
                    http->state = 31;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 31:  // optioNs
                if ((http->c=='N') || (http->c=='n')) {
                    http->len++;
                    http->state = 32;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 32:  // optionS
                if ((http->c=='S') || (http->c=='s')) {
                    http->len++;
                    http->state = 3;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 33:  // cOnnect
                if ((http->c=='O') || (http->c=='o')) {
                    http->len++;
                    http->state = 34;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 34:  // coNnect
                if ((http->c=='N') || (http->c=='n')) {
                    http->len++;
                    http->state = 35;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 35:  // conNect
                if ((http->c=='N') || (http->c=='n')) {
                    http->len++;
                    http->state = 36;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 36:  // connEct
                if ((http->c=='E') || (http->c=='e')) {
                    http->len++;
                    http->state = 37;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 37:  // conneCt
                if ((http->c=='C') || (http->c=='c')) {
                    http->len++;
                    http->state = 2;
                } else {
                    http->method = 0;
                    error=-1;
                }
                break;
            case 38: //  end of header fields. Check 0x0a
                if (http->c=='\x0a') {
                    if (http->contentlen > 0) {
                        if ((http->contenttype != 0) && (strcasecmp(http->contenttype, "application/x-www-form-urlencoded") == 0)) {
                            http->state = 44;
                        } else {
                            http->state = 39;
                        }
                    } else {
                        http->state = -1;
                    }
                } else {
                    http->state = -1;
                    error = -1;
                }
                http->len++;
                break;
            case 39:  //  now copy the content to its buffer
                http->content[http->contentcount] = http->c;
                http->contentcount++;
                if (http->contentcount>=http->contentlen) {
                    http->state = -1;
                }
                break;
            case 40:
                if (http->c == ' ') {
                    http->buffer[http->len] = '\0';
                    http->state = 5;
                } else {
                    http->name = http->buffer+http->len;
                    http->state = 41;
                }
                http->len++;
                break;
            case 41:  //  wait for end of name
                if (http->c == '=') {
                    http->buffer[http->len] = '\0';
                    http->state = 42;  //  Now the value
                }
                http->len++;
                break;
            case 42:   //parameter value
                if (http->c == '&') {  //  Next parameter name
                    http->buffer[http->len] = '\0';
                    //
                    //  only name set.
                    http->parameter.insert(std::pair<const char*, const char*>(http->name, 0));
                    http->state = 40;
                } else if (http->c == ' ') {
                    http->buffer[http->len] = '\0';
                    //
                    //  only name set.
                    http->parameter.insert(std::pair<const char*, const char*>(http->name, 0));
                    http->state = 5;
                } else {
                    http->value = http->buffer+http->len;
                    http->state = 43;   //  wait for end of value
                }
                http->len++;
                break;
            case 43:   // wait for end of value
                if (http->c == '&') {  // Next parameter name
                    http->buffer[http->len] = '\0';
                    http->state = 40;
                    //
                    //  Name and value set.
                    http->parameter.insert(std::pair<const char*, const char*>(http->name, http->value));
                } else if (http->c == ' ') {
                    http->buffer[http->len] = '\0';
                    http->state = 5;
                    //
                    //  Name and value set.
                    http->parameter.insert(std::pair<const char*, const char*>(http->name, http->value));
                }
                http->len++;
                break;
            case 44:  //  now copy the content to its buffer
                http->content[http->contentcount] = http->c;
                http->contentcount++;
                if (http->contentcount>=http->contentlen) {
                    http->state = -1;
                }
                break;
            default:
                break;
            }
        }
        s++;
    } while ((http->state != -1) && (s-(char*)(aPacket->buffer) < (int)(aPacket->len)) && (error != -1) && (http->len < HTTP_REQUEST_LEN));
    /*
     * If the request has been received completly.
     */
    if (http->state == -1) {
        /*
         * Now let us check what type of content we may have to process.
         */
        if ((strcasecmp(http->method, "post")==0) && (http->contentlen > 0)) {
            /*
             * URL-Encoded form-data
             */
            if ((http->contenttype == 0) || (strcasecmp(http->contenttype, "application/x-www-form-urlencoded") == 0)) {
                int   substate = 41;
                char* name     = 0;
                char* value    = 0;
                /*
                 * We start with a name.
                 */
                s    = http->content;
                name = s;
                /*
                 * Until all content has been processed
                 */
                while ((s-http->content) < http->contentlen) {
                    /*
                     * Copied the parameter processing from the URL
                     */
                    switch (substate) {
                    case 41:  //  wait for end of name
                        if (*s == '=') {
                            *s       = '\0';
                            substate = 42;  /*  Now the value  */
                            value    = s+1;
                        }
                        break;
                    case 42:   //parameter value
                        if (*s == '&') {  //  Next parameter name
                            *s = '\0';
                            //
                            //  only name set.
                            http->parameter.insert(std::pair<const char*, const char*>(name, value));
                            substate = 41;
                            name     = s+1;
                        }
                        break;
                    default:
                        break;
                    }
                    s++;
                }
                /*
                 * If we end the loop and did any content processing we add the last parameter
                 */
                http->parameter.insert(std::pair<const char*, const char*>(name, value));
            } else {
                /*  Here we may extend the content pre-processing. */
            }
        }
    }
    return (error);

}
