#ifndef DB_H
#define DB_H

class CSharedMsgQueue;

namespace mariadb {
    extern CSharedMsgQueue* db;
    extern void open(tCommTarget sender, std::string name, std::string host="", std::string port="", std::string user="", std::string passwd="");
    extern void sqlexecstring(tCommTarget sender, int handle, uint64_t cmdtype, const char* cmd, uint32_t limit = (uint32_t)-1 );
    extern void sqlfetch(tCommTarget sender, int handle, uint32_t limit = (uint32_t)-1);
    extern void close(tCommTarget sender, int handle);
}

namespace pgdb {
    extern CSharedMsgQueue* db;
    extern void open(tCommTarget sender, std::string name, std::string host="", std::string port="", std::string user="", std::string passwd="");
    extern void sqlexecstring(tCommTarget sender, int handle, uint64_t cmdtype, const char* cmd, uint32_t limit = (uint32_t)-1 );
    extern void sqlfetch(tCommTarget sender, int handle, uint32_t limit = (uint32_t)-1);
    extern void close(tCommTarget sender, int handle);
}

namespace stdb {
    extern CSharedMsgQueue* db;

    extern void createobj(int handle, uint64_t objid, uint64_t type) ;
    extern void createfromtemplate(int handle, uint32_t templateid, uint32_t objid, uint64_t type) ;
    extern void deleteobj(int handle, uint64_t objid) ;

    extern void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, double value) ;
    extern void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, double value) ;
    extern void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, double value) ;
    extern void set(int handle, uint64_t objid, uint64_t valueid, uint64_t index, double value);
    extern void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) ;
    extern void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) ;
    extern void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value) ;
    extern void set(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t value);
    extern void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, bool value) ;
    extern void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, bool value) ;
    extern void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, bool value) ;
    extern void set(int handle, uint64_t objid, uint64_t valueid, uint64_t index, bool value);
    extern void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, int64_t value) ;
    extern void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, int64_t value) ;
    extern void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, int64_t value) ;
    extern void set(int handle, uint64_t objid, uint64_t valueid, uint64_t index, int64_t value);
    extern void createdata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, const char*value) ;
    extern void updatedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, const char*value) ;
    extern void deletedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, const char*value) ;
    extern void set(int handle, uint64_t objid, uint64_t valueid, uint64_t index, const char* value);
    extern void createreferencedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t reference) ;
    extern void updatereferencedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index, uint64_t reference) ;
    extern void deletereferencedata(int handle, uint64_t objid, uint64_t valueid, uint64_t index) ;

    extern void open(tCommTarget sender, std::string name, std::string host="", std::string port="", std::string user="", std::string passwd="");
    extern void preparestorage(tCommTarget sender, int handle, bool history, bool versions, bool ondemand);
    extern void sqlexecstring(tCommTarget sender, int handle, uint64_t cmdtype, const char* cmd, uint32_t limit = (uint32_t)-1 );
    extern void sqlfetch(tCommTarget sender, int handle, uint32_t limit = (uint32_t)-1);
    extern void close(tCommTarget sender, int handle);
}
#endif // DB_H
