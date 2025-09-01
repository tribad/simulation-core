#ifndef MAINLOADSIMULATION_H
#define MAINLOADSIMULATION_H


class CMainLoadSimulation : public CMainCtrl
{
public:
    CMainLoadSimulation() : CMainCtrl()  {substate = 0;}
    virtual ~CMainLoadSimulation() {}
    virtual bool Action();
    virtual CMainCtrl::State Process(tSigSQLExecStringReplyPtr msg);
    virtual CMainCtrl::State Process(tSigSQLFetchReplyPtr aMsg);
private:
    void LoadObjects(tSQLRecordSet* aData);
    CMainCtrl::State  LoadData(tSQLRecordSet* aData);
private:
    int substate;
    int columncount;
};

#endif // MAINLOADSIMULATION_H
