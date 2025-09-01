#ifndef SIMOBJFACTORY_H
#define SIMOBJFACTORY_H

#include <simobj.h>

typedef tSimObj*  (*tCreateObject)(objectid_t oid, uint64_t droptime);
typedef tSimObj*  (*tCreateNewFromTemplate)(templateid_t, objectid_t);
typedef tSimObj*  (*tCreateNewObject)(objectid_t);

typedef struct tagObjLib {
    struct tagObjLib    *next;
    char                *path;
    void                *handle;
    typeid_t            tid;
    tCreateObject       createobj;
    tCreateNewObject    createnewobj;
    tCreateNewFromTemplate createnewfromtemplate;
} tObjLib;

extern tObjLib *factorylibs;

void addsimobjfactory(tObjLib *newlib) ;
void loadfactory(const char * path) ;
tSimObj *factory_createobj(typeid_t objtypeid, objectid_t oid, uint64_t droptime) ;
tSimObj *factory_createnewobj(typeid_t objtypeid, objectid_t oid) ;
tSimObj *factory_createfromtemplate(typeid_t tid, templateid_t templateid, objectid_t oid);

#endif // SIMOBJFACTORY_H
