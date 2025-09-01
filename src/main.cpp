// **************************************************************************
//
//  Modul-Name     : main.cpp
//  Author(s)      : Hans-Juergen Lange
//
//  Copyrights 2014 by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <dlfcn.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/signalfd.h>
#include <poll.h>
#include <map>
#include <fstream>
#include <sstream>
#include <libpq-fe.h>
#include <helper.h>
#include <CSharedLibrary.h>
#include <msg.h>
#include <CEvent.h>
#include <msgqueue.h>
#include <CFileName.h>
#include <logthread.h>
#include <logger.h>
#include <netpack.h>
#include <simobj.h>
#include "pdu/wsmessage.h"
#include <simifc.h>
#include <sim.h>
#include <simworker.h>

#include "pdu/httprequest.h"
#include <commthread.h>

#include <timer.h>

#include <cachethread.h>

#include <ifcthread.h>
#include <sslifcthread.h>

#include <jsonconnection.h>
#include <jsonifcthread.h>
#include <phpconnection.h>
#include <phpifcthread.h>
#include <phpsconnection.h>
#include <phpsifcthread.h>
#include <wsconnection.h>
#include <wsifcthread.h>
#include <wssconnection.h>
#include <wssifcthread.h>
#include <httpdomainspec.h>
#include <httpconnection.h>
#include <httpifcthread.h>
#include <httpsconnection.h>
#include <httpsifcthread.h>

#include <binifc.h>
#include <binsifc.h>

#include <db.h>

#include <tSigSQLConnectReply.h>
#include <tSigSQLExecStringReply.h>
#include <tSigSQLFetchReply.h>

#include <mainctrl.h>

#include <version.h>
#include <udpifcthread.h>

/*
 * This is the xml configuration file that replaces the commandline arguments.
 */
static char*       configfilename = 0;
static xmlDocPtr   xmlConfig      = 0;
static const char* defaultConfiguration =
        "<configuration>"
        "<simulation path=\"../prbg/game/prbg.so\"/>"
        "<interfaces>"
        "<interface type=\"php\" ifc=\"0.0.0.0\" port=\"38080\"/>"
        "<interface type=\"phps\" ifc=\"0.0.0.0\" port=\"38888\"/>"
        "<interface type=\"ws\" ifc=\"0.0.0.0\" port=\"5050\"/>"
        "<interface type=\"wss\" ifc=\"0.0.0.0\" port=\"5555\" ca=\"appcore.cert\"/>"
        "<interface type=\"http\" ifc=\"0.0.0.0\" port=\"8080\" contentmax=\"16M\">"
        "<proto redir=\"true\" port=\"8888\"/>"
        "</interface>"
        "<interface type=\"https\" ifc=\"0.0.0.0\" port=\"8888\" contentmax=\"16M\">"
        "<proto>"
        "<domains>"
        "<domain name=\"localhost\" htmldir=\".\" contentdir=\".\" contentlib=\"content.so\">"
        "</domain>"
        "</domains>"
        "</proto>"
        "</interface>"
        "</interfaces>"
        "<database type=\"postgresql\" host=\"odin\" user=\"prbgadmin\" port=\"5432\">"
        "<threads>"
        "<storage/>"
        "</threads>"
        "</database>"
        "</configuration>";

int  mainpid = 0;
bool persistent=false;
/*
 * These are the websocket parameter.
 */
std::string websocketifc        = "0.0.0.0";
int         websocketport       = 5050;
std::string securewebsocketifc  = "0.0.0.0";
int         securewebsocketport = 5555;
/*
 * Some information that define the connection to the web frontend.
 */
bool usetcp=true;
bool useudp=false;
/*
 * This is the port number the listener is waiting on for incoming
 * connections. Maybe overwritten with the program parameter -p
 */
const char* listenifc  = "0.0.0.0";
/*
 * The simulation core data.
 */
extern tSimCore core;
/*
 * Define a loglevel that is used to decide whether a message is
 * put on cerr or not.
 */
uint64_t loglevel;
bool     debugws  = false;
bool     debugweb = false;

CSharedLibrary* pgdblib    = 0;
CSharedLibrary* mariadblib = 0;
/*
 * How the simulation itself is run
 */
bool     freerunning = false;
/*
 * Whether to display a message each cycle or not.
 */
bool cycleoutput = false;
/*
 * The simulation can be stopped through the running variable.
 */
bool running = true;
/*
 * A vector to hold all interface threads.
 */
static std::vector<CIfcThread*> Interfaces;
/*
 * UDP threads are somewhat different and get hold in another vector
 */
static std::vector<CUdpInterface*> UdpInterfaces;
/*
 * The number of cycles to run. This is limited to the maximum value
 * a uint64_t var can hold.
 */
uint64_t maxcycle    = UINT64_MAX;
/*
 * static function prototypes, sometime needed.
 */
/*
 * This is a function to process some long parameter.
 */
static void process_long_parameter(const char* lp) {
    std::string longparameter=lp;
    std::string name=longparameter.substr(0, longparameter.find_first_of('='));
    std::string value=longparameter.substr(longparameter.find_first_of('=')+1);

    if (name == "websocket-interface") {
        websocketifc=value;
    } else if (name == "websocket-port") {
        websocketport = strtol(value.c_str(), 0, 0);
    } else if (name == "secure-websocket-interface") {
        securewebsocketifc=value;
    } else if (name == "secure-websocket-port") {
        securewebsocketport = strtol(value.c_str(), 0, 0);
    } else if (name=="not-persistent") {
        persistent=false;
    } else if (name=="free-running") {
        freerunning=true;
    } else if (name=="no-cycle-output") {
        cycleoutput = false;
    } else if (name == "cycle-output") {
        cycleoutput = true;
    } else if (name == "debug-json-in") {
        debugws  = true;
        debugweb = true;
    } else {
        std::cerr << "unknown parameter : " << name << "\n";
    }
}
static void ReadInHTTPProto(xmlNode* proto, CLogger* aMainLogger) {
    //
    //  Search the proto node.
    while (proto != 0) {
        if (proto->type == XML_ELEMENT_NODE) {
            if (helper::tolower(std::string((const char*)(proto->name))) == "proto") {
                //
                //  Searching the "domains" tag
                xmlNode* domains = proto->children;
                while (domains != 0) {
                    if (domains->type == XML_ELEMENT_NODE) {
                        if (helper::tolower(std::string((const char*)(domains->name))) == "domains") {
                            //
                            //  Searching the "domain" tags
                            xmlNode* domain = domains->children;
                            while (domain != 0) {
                                if (domain->type == XML_ELEMENT_NODE) {
                                    if (helper::tolower(std::string((const char*)(domain->name))) == "domain") {
                                        std::string name;
                                        std::string htmldir;
                                        std::string contentdir;
                                        std::string datadir;
                                        std::string contentlib;
                                        std::string maxage;
                                        //
                                        //  Handle the domain spec.
                                        name       = helper::xml_string_property(domain, "name");
                                        htmldir    = helper::xml_string_property(domain, "htmldir");
                                        contentdir = helper::xml_string_property(domain, "contentdir");
                                        datadir    = helper::xml_string_property(domain, "datadir");
                                        contentlib = helper::xml_string_property(domain, "contentlib");
                                        maxage     = helper::xml_string_property(domain, "maxage");
                                        //
                                        //  Check the name if it already exists.
                                        //  Doing it here is more logical. We have no benefit if we do check earlier.
                                        //  Its a matter of readability.
                                        if (CHTTPProtocol::domains.find(name) == CHTTPProtocol::domains.end()) {
                                            CHttpDomainSpec* spec = new CHttpDomainSpec(name, htmldir, contentdir, datadir, contentlib, maxage, aMainLogger);
                                            CHTTPProtocol::domains.insert(std::pair<std::string, CHttpDomainSpec*>(name, spec));
                                            //
                                            //  Here we read in aliases. They store the same pointer
                                            //  with the alias into the domain-map.
                                            xmlNode* alias = helper::xml_find_child(domain, "alias");
                                            if (alias != 0) {
                                                std::string aliases = helper::xml_content(alias);
                                                //
                                                //  Split the content into the aliases.
                                                std::string onealias;
                                                size_t      start    = 0;
                                                size_t      end      = 0;

                                                do {
                                                    end      = aliases.find_first_of(" ,;:\n", start);
                                                    onealias = aliases.substr(start, end-start);
                                                    /*
                                                     * trim front
                                                     */
                                                    while (onealias[0] == ' ') onealias.erase(0, 1);
                                                    /*
                                                     * if proto has a size after trimming we can add it to the
                                                     * list of protos.
                                                     */
                                                    if (onealias.size() > 0) {
                                                        CHTTPProtocol::domains.insert(std::pair<std::string, CHttpDomainSpec*>(onealias, spec));
                                                    }
                                                    /*
                                                     * if we are not at end of search we skip a character.
                                                     * At end we do nothing and let the loop condition stop
                                                     * the loop.
                                                     */
                                                    if (end == std::string::npos) {
                                                        start = end;
                                                    } else {
                                                        start = end + 1;
                                                    }
                                                } while (end != std::string::npos);
                                            }
                                            /*
                                             * Search parameter node
                                             */
                                            xmlNode* domainparam = helper::xml_find_child(domain, "parameter");
                                            if (domainparam != 0) {
                                                spec->Parameter = domainparam;
                                            }
                                        }

                                    }
                                }
                                domain = domain->next;
                            }
                        }
                    }
                    domains = domains->next;
                }
            }
        }
        proto = proto->next;
    }
}
//
//  This function reads the global table from a file for extensions
//  To mime-type mapping.
static void ReadHTTPMimeTypes(void) {
    std::ifstream mimefile("/usr/share/mime/globs");
    std::string line;

    while (mimefile.good()) {
        std::getline(mimefile, line, '\n');
        if (!line.empty()) {
            /*
             * trim front
             */
            while (line[0] == ' ') line.erase(0, 1);
            //
            //  Check for comment line
            if (line[0] != '#') {
                //
                //  Search the colon from end19
                size_t colonpos = line.find_last_of(':');

                if (colonpos != std::string::npos) {
                    std::string mime = line.substr(0, colonpos);
                    //
                    //  Skip the colon, asterisk and dot
                    std::string ext  = line.substr(colonpos+3);
                    if (CHTTPProtocol::ext2mime.find(ext) == CHTTPProtocol::ext2mime.end()) {
                        CHTTPProtocol::ext2mime.insert(std::pair<std::string, std::string>(ext, mime));
                    }
                }
            }
        }
    }
}
/*
 * Main entry point.
 */
int main(int argc, char* argv[], char* envp[]) {
    //
    //
    (void)envp;
    //
    //  get the pid to use later on in some signaling functions.
    //  and to get the path to the application.
    mainpid=getpid();
    char        path[PATH_MAX];
    char        dest[PATH_MAX];
    CFileName   application;
    CFileName   pglibpath;
    CFileName   marialibpath;
    //
    //  This is the only way on a linux system to get the real path to the application even if
    //  it gets started through various links.
    pid_t pid = getpid();
#ifdef __linux
    sprintf(path, "/proc/%d/exe", pid);
    if (readlink(path, dest, PATH_MAX) == -1) {
        application=argv[0];
    } else {
        application=dest;
    }
#else
    application=argv[0];
#endif
    if ((application.Directory() == "/usr/local/bin/") || (application.Directory() == "/usr/bin/")) {
        pglibpath = application.Directory();
        pglibpath.ReduceDepth();

#if ( __x86_64__ == 0)
        pglibpath = pglibpath + "lib/";
#else
        pglibpath = pglibpath + "lib64/";
#endif
        marialibpath = pglibpath + "libmariastorage-" APPCORE_VERSION ".so";
        pglibpath    = pglibpath + "libpgsqlstorage-" APPCORE_VERSION ".so";
    } else {
        pglibpath = application.Directory() + "src/components/pgsql/libpgsqlstorage-" APPCORE_VERSION ".so";
        marialibpath = application.Directory() + "src/components/mariadb/libmariastorage-" APPCORE_VERSION ".so";
    }
    /*
     * First we must block all signals that we want to handle in the threads
     * As the signal-mask gets inherited to the threads we dont need to block the signals
     * per thread. Only activating what is used.
     */
    sigset_t signals;

    sigemptyset(&signals);
    for (int i=SIGRTMIN;i<SIGRTMAX;++i) {
        sigaddset(&signals, i);
    }
    sigaddset(&signals, SIGHUP);
    sigaddset(&signals, SIGPIPE);
    sigaddset(&signals, SIGURG );
    sigaddset(&signals, SIGIO);

    int err = sigprocmask(SIG_BLOCK , &signals, NULL);
    /*
     * We need the signals infrastructure and blocking them on startup is essentiell for
     * the function of the software. So we check the result and terminate on error.
     */
    if (err != 0) {
        std::cerr << "Cannot block the signals and thus terminating. " << strerror(errno) << std::endl;
        exit(-1);
    }
    //
    //  The first thing we do is starting the logging thread.
    CLogThread::Instance.Create();
    while (!CLogThread::Instance.Running) {}
    //
    //  Than we create the logger for the main-thread.
    CLogger mainlogger(ModulId::Main, "MainThread");
    mainlogger.Write(LogLevel::Unbreakable, "The Simulated-Universe application core is starting up.");
    mainlogger.Write(LogLevel::Unbreakable, "Copyright by Hans-Jürgen Lange <hjl@simulated-universe.de>");
    mainlogger.Write(LogLevel::Trace, "Using %s as path to the postgresql storage library", ((std::string)(pglibpath)).c_str());
    mainlogger.Write(LogLevel::Trace, "Using %s as path to the mariadb storage library", ((std::string)(marialibpath)).c_str());
    //
    //  Initialization of the random number generator.
    srand(4711);
    //
    //  Initialize the gnu-tls lib.
    if (gnutls_global_init() != 0) {
        mainlogger.Write(LogLevel::Critical, "Cannot initialize the GNU-TLS library");
        exit(0);
    }
    //
    //  Initialize the program parameter parser part.
    int         i       = 1;
    std::string simpath;
     /*
     * block all signals that may be used for communication an the like.
     */
    mainlogger.Write(LogLevel::Debug, "RT-Signal Min: %i RT-Signal Max: %i", SIGRTMIN, SIGRTMAX);
    /*
     * Parse the program parameters.
     */
    while ((i<argc) && (argv[i]!=0)) {
        char *s=argv[i];
        if (*s=='-') {
            s++;
            switch (*s) {
            case 'w':
                s++;
                if (*s!='\0') {
                    simsetnumberofworkers(atoi(s));
                } else {
                    i++;
                    s=argv[i];
                    if (s!=0) {
                        simsetnumberofworkers(atoi(s));
                    } else {
                    }
                }
                break;
            case 'l':
                s++;
                if (*s!='\0') {
                    loglevel = strtol(s, 0, 0);
                } else {
                    i++;
                    s=argv[i];
                    if (s!=0) {
                        loglevel = strtol(s, 0, 0);
                    } else {
                    }
                }
                break;
            case 'm':   //  maximum cycles
                s++;
                if (*s!='\0') {
#if (__SIZEOF_LONG__ == 8)
                    maxcycle = strtoul(s, 0, 0);
#else
#if (__SIZEOF_LONG_LONG__ == 8)
                    maxcycle = strtoull(s, 0, 0);
    #else
        #error "No valid conversion to uint64_t found"
    #endif
#endif
                } else {
                    i++;
                    s=argv[i];
                    if (s!=0) {
#if (__SIZEOF_LONG__ == 8)
                    maxcycle = strtoul(s, 0, 0);
#else
    #if (__SIZEOF_LONG_LONG__ == 8)
                    maxcycle = strtoull(s, 0, 0);
    #else
        #error "No valid conversion to uint64_t found"
    #endif
#endif
                    } else {
                    }
                }
                break;
            case 't':
                usetcp=true;
                useudp=false;
                break;
            case 'u':
                usetcp=false;
                useudp=true;
                break;
            case 'L':
                s++;
                if (*s!='\0') {
                    listenifc=strdup(s);
                } else {
                    i++;
                    s=argv[i];
                    if (s!=0) {
                        listenifc=strdup(s);
                    } else {
                    }
                }
                break;
            case 's':
                s++;
                if (*s != '\0') {
                    simpath = s;
                } else {
                    i++;
                    s = argv[i];
                    if (s != nullptr) {
                        simpath = s;
                    } else {
                    }
                }
                break;
            case 'd': // DB name
                s++;
                if (*s!='\0') {
                    CMainCtrl::dbname=strdup(s);
                } else {
                    i++;
                    s=argv[i];
                    if (s!=0) {
                        CMainCtrl::dbname=strdup(s);
                    } else {
                    }
                }
                break;
            case '-':    //  Long parameter
                s++;     //  Move on to parameter name
                process_long_parameter(s);
                break;
            case 'c':
                s++;
                if (*s!='\0') {
                    configfilename=strdup(s);
                } else {
                    i++;
                    s=argv[i];
                    if (s!=0) {
                        configfilename=strdup(s);
                    } else {
                    }
                }
                //
                //  Check if we have an config filename
                if (configfilename != 0) {
                    xmlConfig = xmlReadFile(configfilename, NULL, 0);
                }
                break;
            default:
                break;
            }
        }
        i++;
    }
    /*
     * Start the timer thread.
     */
    starttimerthread();
    /*
     * start cache thread
     */
    CCacheThread::instance().Create();

    while (CCacheThread::instance().Running == false);
    //
    //  Starting a secondary comm thread.
    CCommThread::Start();
    /*
     * Wait until it reaches the run loop.
     */
    coreIfc.comm = &(CCommThread::instance().q);
    /*
     * If we have no configuration we use a build-in one.
     */
    if (xmlConfig == NULL) {
        xmlConfig = xmlReadMemory(defaultConfiguration, strlen(defaultConfiguration), "default.xml", NULL, 0);
    }
    /*
     * Only if the parsing has any success, with a file or with the default configuration
     * we continue with the initialization.
     */
    if (xmlConfig != NULL) {
        /*
         * Do not free the xml-document. Parts of it may be passed into the contentlibraries.
         */
        xmlNode* root=xmlDocGetRootElement(xmlConfig);
        /*
         * We go along the root to find the configuration node.
         */
        while (root != 0) {
            if (root->type == XML_ELEMENT_NODE) {
                /*
                 * Search the configuration node.
                 */
                if (helper::tolower(std::string((const char*)(root->name))) == "configuration") {
                    xmlNode* configuration = root->children;
                    /*
                     * go along the configuration nodes.
                     */
                    while (configuration != nullptr) {
                        if (configuration->type == XML_ELEMENT_NODE) {
                            std::string nodename=helper::tolower(std::string((const char*)(configuration->name)));
                            /*
                             * Check for one of the sub-nodes.
                             */
                            if (nodename == "simulation") {
                                if (simpath.empty()) {
                                    xmlChar *path = xmlGetProp(configuration, (const xmlChar *) "path");

                                    simpath = (const char *) path;
                                    if (path != 0) {
                                        xmlFree(path);
                                    }
                                }
                                freerunning = helper::xml_bool_property(configuration, "freerunning", false);
                                //
                                //  this is the cycle time in ms.
                                core.stepsize = helper::xml_long_property(configuration, "cycletime");
                                if (core.stepsize == 0) {
                                    core.stepsize =1000;
                                }

                                core.updatechunksize = helper::xml_long_property(configuration, "updatechunksize");
                                if (core.updatechunksize == 0) {
                                    core.updatechunksize =1024;
                                }
#if 0
                                core.deletechunksize = helper::xml_long_property(configuration, "deletechunksize");
                                if (core.deletechunksize == 0) {
                                    core.deletechunksize = 128;
                                }
#endif
                                core.signalchunksize = helper::xml_long_property(configuration, "signalchunksize");
                                if (core.signalchunksize == 0) {
                                    core.signalchunksize = 1024;
                                }

                            } else if (nodename == "interfaces") {
                                xmlNode* interface = configuration->children;

                                while (interface != 0) {
                                    if (interface->type == XML_ELEMENT_NODE) {
                                        if (helper::tolower(std::string((const char*)(interface->name))) == "interface") {
                                            std::string ifctype;
                                            std::string ifcid;
                                            std::string ifcca;
                                            std::string ifccert;
                                            std::string ifckey;

                                            int         ifcport   = 0;
                                            int         ifccertupdatetime = 1;

                                            ifctype = helper::xml_string_property(interface, "type");
                                            ifcid   = helper::xml_string_property(interface, "ifc");
                                            ifcport = helper::xml_long_property(interface, "port");
                                            ifcca   = helper::xml_string_property(interface, "ca");
                                            ifccert = helper::xml_string_property(interface, "cert");
                                            ifckey  = helper::xml_string_property(interface, "key");


                                            ifccertupdatetime = helper::xml_long_property(interface, "certupdatetime", 1);

                                            if (ifctype == "php") {
                                                CPHPIfcThread* ifcthread = new CPHPIfcThread;
                                                ifcthread->Create(ifcid, ifcport);
                                                Interfaces.push_back(ifcthread);
                                            } else if (ifctype == "phps") {
                                                CPHPsIfcThread* ifcthread = new CPHPsIfcThread;
                                                ifcthread->Create(ifcid, ifcport, ifccert, ifckey, ifcca, ifccertupdatetime);
                                                Interfaces.push_back(ifcthread);
                                            } else if (ifctype == "json") {
                                                CJsonIfcThread* ifcthread = new CJsonIfcThread;
                                                ifcthread->Create(ifcid, ifcport);
                                                Interfaces.push_back(ifcthread);
                                            } else if (ifctype == "ws") {
                                                CWSIfcThread* ifcthread = new CWSIfcThread;
                                                ifcthread->Create(ifcid, ifcport);
                                                Interfaces.push_back(ifcthread);
                                            } else  if (ifctype == "udp") {
                                                std::string serverName = helper::xml_string_property(interface, "name");

                                                CUdpInterface* ifcthread = new CUdpInterface(ModulId::UdpIfc, "UDP-Server");
                                                ifcthread->Create(ifcid, ifcport, serverName);
                                                UdpInterfaces.push_back(ifcthread);
                                            } else if (ifctype == "wss") {
                                                CWSSIfcThread* ifcthread = new CWSSIfcThread;
                                                ifcthread->Create(ifcid, ifcport, ifccert, ifckey, ifcca, ifccertupdatetime);
                                                Interfaces.push_back(ifcthread);
                                            } else if (ifctype == "http") {
                                                //
                                                //  For the HTTP protocol we have a lot of configuration infos
                                                ReadInHTTPProto(interface->children, &mainlogger);
                                                ReadHTTPMimeTypes();
                                                //
                                                //  Now start the thread
                                                CHTTPIfcThread* ifcthread = new CHTTPIfcThread;
                                                ifcthread->Create(ifcid, ifcport);
                                                Interfaces.push_back(ifcthread);
                                            } else if (ifctype == "https") {
                                                //
                                                //  For the HTTPs protocol we have a lot of configuration infos
                                                ReadInHTTPProto(interface->children, &mainlogger);
                                                ReadHTTPMimeTypes();
                                                //
                                                //  Now start the threads
                                                CHTTPSIfcThread* ifcthread = new CHTTPSIfcThread;
                                                ifcthread->Create(ifcid, ifcport, ifccert, ifckey, ifcca, ifccertupdatetime);
                                                Interfaces.push_back(ifcthread);
                                            }
                                        }
                                    }
                                    interface = interface->next;
                                }
                            } else if (nodename == "database") {
                                CMainCtrl::dbtype   = helper::xml_string_property(configuration, "type");
                                CMainCtrl::dbname   = helper::xml_string_property(configuration, "name");
                                CMainCtrl::dbhost   = helper::xml_string_property(configuration, "host");
                                CMainCtrl::dbport   = helper::xml_string_property(configuration, "port");
                                CMainCtrl::dbuser   = helper::xml_string_property(configuration, "user");
                                CMainCtrl::dbpasswd = helper::xml_string_property(configuration, "passwd");
                            }
                        }
                        configuration = configuration->next;
                    }
                }
            }
            root=root->next;
        }
    }
    //
    //  Load the DB connection libraries.
    //
    //  First we need a function pointer defintion
    //  for the init function of the db libs.
    using FunctionPtr = CSharedMsgQueue* (*)(CSharedMsgQueue*);

    FunctionPtr initfnc;
    //
    //  Because we setup the queues we need that definition too.
    CSharedMsgQueue   mainQ;
    //
    //  To send the log-messages to the logger we need a stream object.
    std::ostringstream oss;
    //
    //  First we do the PostgreSQL DB
    pgdblib = new CSharedLibrary(pglibpath, oss);
    if (oss.str().size() > 0) {
        if ((pgdblib != nullptr) && (pgdblib->IsLoaded())) {
            mainlogger.Write(LogLevel::Info, oss.str().c_str());
        } else {
            mainlogger.Write(LogLevel::Error, oss.str().c_str());
        }
        oss.clear();
        oss.str("");
    }
    if ((pgdblib != nullptr) && (pgdblib->IsLoaded())) {
        initfnc = (FunctionPtr)(pgdblib->GetFunction("init"));
        if (initfnc != nullptr) {
            pgdb::db = initfnc(&CCommThread::instance().q);
            if (pgdb::db == nullptr) {
                mainlogger.Write(LogLevel::Error, "Init of mariadb library failed");
            }
        } else {
            mainlogger.Write(LogLevel::Error, "Could not find init function : %s", dlerror());
        }
    } else {
        pgdb::db = nullptr;
    }
    //
    //  Then the mariadb
    mariadblib = new CSharedLibrary(marialibpath, oss);
    if (oss.str().size() > 0) {
        if ((mariadblib != nullptr) && (mariadblib->IsLoaded())) {
            mainlogger.Write(LogLevel::Info, oss.str().c_str());
        } else {
            mainlogger.Write(LogLevel::Error, oss.str().c_str());
        }
        oss.clear();
        oss.str("");
    }
    if ((mariadblib != nullptr) && (mariadblib->IsLoaded())) {
        initfnc = (FunctionPtr)(mariadblib->GetFunction("init"));
        if (initfnc != nullptr) {
            mariadb::db = initfnc(&CCommThread::instance().q);
            if (mariadb::db == nullptr) {
                mainlogger.Write(LogLevel::Error, "Init of mariadb library failed");
            }
        } else {
            mainlogger.Write(LogLevel::Error, "Could not find init function : %s", dlerror());
        }
    } else {
        mariadb::db = nullptr;
    }
    //
    //  setup the storage db variables.
    if (CMainCtrl::dbtype == "postgresql") {
        stdb::db = pgdb::db;
    } else if (CMainCtrl::dbtype == "mariadb") {
        stdb::db = mariadb::db;
    } else {
        //
        //  We use no storage db right from the start.
    }
    //
    //  Setup the queues in the coreifc
    coreIfc.stdb    = stdb::db;
    coreIfc.mariadb = mariadb::db;
    coreIfc.pgdb    = pgdb::db;
    /*
     *  Initialization of the simulation. Its all read from the database.
     *  Simulation libraries are loaded as well here because they contain the simulation
     *  object code.
     *  The function returns true if everything worked fine. False if not.
     */
    if (init_simulation(simpath, mainlogger)) {
        /*
         * Loading of configuration data from the DB is not implemented for now.
         * So we set some values for the simulation core here.
         */
        core.endcycle = -1;
    } else {
        mainlogger.Write(LogLevel::Error, "Could not initialize simulation. Aborting\n");
        exit(-1);
    }
    //
    //  This is our big statemachine to manage all internals.
    std::shared_ptr<tMsg> msg;
    //
    //  Before running the main loop we setup the mainthread target.
    CMainCtrl::mainthread  = tCoreQueue(&mainQ);

    for (;;) {
        if (CMainCtrl::DoAction()) {
            mainQ.Wait();
            msg = mainQ.Get();
            if (msg != 0) {
                CMainCtrl::Process(msg);
            }
        }
    }
    return (0);
}

