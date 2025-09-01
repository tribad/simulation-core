#ifndef MAINSTARTSIMDB_H
#define MAINSTARTSIMDB_H


class CMainStartSimDB : public CMainCtrl
{
public:
    CMainStartSimDB() : CMainCtrl()  {substate = 0;}
    virtual ~CMainStartSimDB() {}
    virtual bool Action();
    virtual CMainCtrl::State Process(tSigSQLConnectReplyPtr aMsg);
    virtual CMainCtrl::State Process(tSigPrepareStorageReplyPtr aMsg);
private:
    int substate;
};

#endif // MAINSTARTSIMDB_H
