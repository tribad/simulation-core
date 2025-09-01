#include <CThread.h>
#include <CEvent.h>
#include <msg.h>
#include <sharedmsgqueue.h>
#include <sharedmsgthread.h>

CSharedMsgThread::CSharedMsgThread()
{

}

long CSharedMsgThread::Run(void) {
    std::shared_ptr<tMsg>  msg;

    Running = true;
    for (;Running;) {
        //
        //  We wait sometime. And if we got a message in time we process the message.
        //  If the wait time expires, we do some special processing.
        if (q.Wait(waittimeout)) {
            msg = q.Get();
            if (msg != 0) {
                Process(msg);
            }
        } else {
            ProcessWaitTimeout();
        }
    }
    return 0;
}
