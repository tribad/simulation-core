#include <msg.h>
#include <sim.h>
#include <mainctrl.h>
#include <mainrunning.h>

int CMainRunning::Process(std::shared_ptr<tMsg> msg) {
    (void)msg;
    return CMainCtrl::eRunning;
}
