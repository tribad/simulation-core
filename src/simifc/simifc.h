#ifndef SIMIFC_H
#define SIMIFC_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <set>
#include <string>
#include <iostream>
#include <coreifc.h>
#include <netpack.h>
#include <simobj.h>

#define VALUEINDEX_NONE ((valueindex_t)(0xffffffff))

extern std::map<uint64_t, uint16_t>         towebmap;
extern std::map<uint16_t, uint64_t>         fromwebmap;
extern std::set<std::string>                wsurlmap;
extern std::set<std::string>                wsprotomap;

typedef std::map< uint64_t, tVariant >::iterator tVariantMapIter;
/*
 *  Here starts the new SimulationInterface
 *  It collects all methods and attributes that are made available to the application core
 *
 *  An instance of this class must be created in the simulation. It needs not as complex as the
 *  generated interface is.
 */
class CSimIfc {
public:
    CSimIfc() {}
    virtual ~CSimIfc() {}
    virtual int       CreateObject(uint64_t oid, uint64_t tid, uint64_t droptime = UINT64_MAX) = 0;
    virtual void      SetValue(uint64_t oid, uint64_t vid, uint64_t vidx, const tVariant& value) {
        (void)oid;
        (void)vid;
        (void)vidx;
        (void)value;
    }
    virtual void      StartCycle() {}
    virtual void      EndCycle() {}
    virtual tMsg*     Process(std::shared_ptr<tMsg> aMsg) {(void) aMsg; return 0;}
    virtual tMsg*     Process(tCommTarget& src, tJSON* aJson) {(void)src; delete aJson; return 0;}
    virtual tNetPack* Process(tNetPack* aPackage) {delete aPackage; return 0;}
    virtual tNetPack* ProcessRaw(tNetPack* aPackage) {delete aPackage; return 0;}
    virtual std::string GetMessageName(uint64_t aId) {(void)aId; return "";}
    virtual tMsg*     GetMessage(tJSON* aJson) { delete aJson; return 0;}  // prevent memory leaks.
public:
    /*
     * These are some flags that define what special features are expected
     * to exist on the DB.
     */
    bool                 UseHistory;
    bool                 UseVersion;
    bool                 UseOnDemandLoading;
};
/*
 * These are functions that the simulation must provide.
 */
extern CSimIfc* siminit(void);

void stdb_set(uint8_t attrtype, objectid_t objid, valueid_t valueid, valueindex_t index, const tVariant&);
void stdb_del(uint8_t attrtype, objectid_t objid, valueid_t valueid, valueindex_t index);


#endif // SIMIFC_H
