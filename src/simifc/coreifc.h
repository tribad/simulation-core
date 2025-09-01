#ifndef COREIFC_H
#define COREIFC_H

#include <simobj.h>

class CMsgQueue;
class CSharedMsgQueue;

class CCoreIfc
{
public:
    CCoreIfc();
    virtual ~CCoreIfc();
    //
    //  StartObject puts the object into the Update-Tables to let
    //  it run in a time-discret fashion.
    //  The itself decides if and when it needs no further updates.
    virtual void StartObject(const tObjectRef &obj);
    //
    //  FreeObject removes the object from the simulation and frees the
    //  associated memory, but only for the object itself.
    //  This means not that the object gets deleted from the DB. This function
    //  is needed to manage the memory and may be called even for objects that are
    //  loaded on demand and are not further on needed.
    virtual void FreeObject(const tObjectRef &obj);
    //
    //  Adding a signal into the schedule table so that they can be processed within
    //  a cycle or with a delay.
    virtual void ScheduleSignal(std::shared_ptr<tSig> aSig, uint64_t aDelay = 0u);
    //
    //  Adding a messagec into the schedule table so that they can be processed within
    //  a cycle or with a delay.
    virtual void ScheduleMsg(std::shared_ptr<tMsg> aMsg, uint64_t aDelay = 0u);
public:
    //
    //  List of message queues to the important components of the core.
    CSharedMsgQueue *stdb       = nullptr; //  To the database that stores the simulation objects
    int             stdbhandle = 0;       //  The handle to the stdb
    CSharedMsgQueue *pgdb       = nullptr; //  To the postgresql db interface
    CSharedMsgQueue *mariadb    = nullptr; //  To the mariadb interface
    CSharedMsgQueue *comm       = nullptr; //  To the comm thread (dispatching messages)
    //
    //  Cycle Counter. Within a cycle they do not change.
    //  But maybe they get changed from different threads.
    volatile uint64_t   relCycle = 0ul;   //  Cycle count from startup not settable
    volatile uint64_t   absCycle = 0ul;   //  Cycle count in absolut value. Can be changed from the simulation.
};

extern CCoreIfc* __coreIfc;

#endif // COREIFC_H
