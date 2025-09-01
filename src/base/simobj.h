/*
 * simobj.h
 *
 *  Created on: Jun 13, 2014
 *      Author: ltoo
 */
#ifndef SIMOBJ_H_
#define SIMOBJ_H_
/*
 * Additional headerfiles needed everywhere
 */
#include <map>
#include <stdint.h>
#include <string.h>
#include <variant.h>
#include <msg.h>
#ifdef __USE_GROBJECT
    #include <grobject.h>
    #include <grstream.h>
#endif

#define INVALID_OBJID INVALID_OBJECTID

#include <objectref.h>
#include <membervalue.h>
/*
 * Forward declaration of the object base structure.
 */
struct tSimObj;
class  tMsg;
/*
 * Definitions of types that describe the function
 * of the type.
 */
typedef uint64_t typeid_t;
typedef uint64_t valueid_t;
typedef uint64_t objectid_t;
typedef uint64_t valueindex_t;
typedef uint64_t templateid_t;
/*
 * Definition of some pointer to function types.
 * They are used to be handled in the simulation.
 */
typedef int       (*tSetValue)(tSimObj*, valueid_t valueid, valueindex_t valueindex, const tVariant& value);
typedef tVariant  (*tGetValue)(tSimObj*, valueid_t valueid, valueindex_t valueindex);
typedef int       (*tSave)(tSimObj*, uint64_t cycle, ...);
typedef void      (*tUpdate)(tSimObj *, uint64_t);
typedef void      (*tProcess)(tSimObj*, tMsg*);
typedef tMsg*     (*tSyncProcess)(tSimObj*, tMsg*);
/*
 * This is the basic simulation object structure
 */
struct tSimObj {
    tSimObj();
    virtual ~tSimObj() ;
    objectid_t       objid = INVALID_OBJECTID;  /* This is a unique identifier of an object.                      */
    typeid_t         type;                      /* This is a unique identifier of the object-type                 */
    uint64_t         parenttype = 0ul;          /* Only an object of this type can be set as parent.              */
    tObjectRef       parent;                    /* This is the pointer to the parent. Maybe useless               */
    tSave            save;                      /* A pointer to a function that can save the object state         */
    tMemberValue     state;                     /* The state of the object                                        */
    uint64_t         droptime = UINT64_MAX;     /* This is the unload timeout time coming from dbs onDemand field */

    void Start();                               /* Add this object to the worker queue.                           */

    virtual bool     update(uint64_t aCycle);   /* Run the statemachine of an object.                             */

    virtual tMsg* Process(std::shared_ptr<tMsg> msg);

    void SetValue(valueid_t vid, const tVariant& value) {SetValue(vid, 0ul, value);}
    virtual void SetValue(valueid_t vid, valueindex_t vidx, const tVariant& value);
    virtual tVariant GetValue(valueid_t vid, valueindex_t vidx = 0ul);

    void SetReference(uint64_t vid, const tObjectRef& ref) {SetReference(vid, 0ul, ref);}
    virtual void SetReference(uint64_t vid, uint64_t vidx, const tObjectRef& ref);
    virtual tObjectRef GetReference(uint64_t vid, uint64_t vidx=0ul);
    virtual void       RemoveReference(uint64_t vid, uint64_t vidx=0ul);

    virtual void InitMember(uint64_t vid, uint64_t vidx, const tVariant& value);

    const tObjectRef& GetParent() {return parent;}
    virtual void SetParent(const tObjectRef& obj);

};

#endif /* SIMOBJ_H_ */
