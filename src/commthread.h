#ifndef __COMMTHREAD_INC__
#define __COMMTHREAD_INC__

#include <sharedmsgthread.h>

class tJSON;
//
//  From here on we define a new comm thread.
class CCommThread : public CSharedMsgThread {
public:
    CCommThread();
    virtual ~CCommThread() {}
    void Process(std::shared_ptr<tMsg> aMsg) override;

    void Put (uint64_t aConnId, tJSON* aJson);
    void Put (uint64_t aConnId, tMsg*  aMsg);
public:  //  static methods here.
    static void Start();
    static CCommThread& instance() {
        static CCommThread comm;

        return comm;
    }
public:
    CLogger             commlogger;
};

#endif
