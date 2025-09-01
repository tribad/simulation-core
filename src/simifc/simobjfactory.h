// MIT License
//
// Copyright (c) 2014 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
