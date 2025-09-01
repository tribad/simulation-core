#ifndef MAINRUNNING_H
#define MAINRUNNING_H


class CMainRunning : public CMainCtrl
{
public:
    CMainRunning() : CMainCtrl() {}
    virtual ~CMainRunning() {}
    virtual int Process(std::shared_ptr<tMsg> msg);
};

#endif // MAINRUNNING_H
