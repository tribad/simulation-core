#ifndef MAINSTARTSTORAGEDB_H
#define MAINSTARTSTORAGEDB_H


class CMainStartStorageDB : public CMainCtrl
{
public:
    CMainStartStorageDB() : CMainCtrl()  {}
    virtual ~CMainStartStorageDB() {}
    virtual bool Action();
    virtual CMainCtrl::State Process(tSigSQLConnectReplyPtr aMsg);
};

#endif // MAINSTARTSTORAGEDB_H
