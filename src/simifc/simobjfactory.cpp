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

#include <stdio.h>
#include <malloc.h>
#include <dlfcn.h>
#include <string.h>
#include <dirent.h>
#include <stdint.h>
#include <msg.h>
#include <simobj.h>
#include <simobjfactory.h>

tObjLib *factorylibs=0;

/*
 * This operation is used as a filter to the scandir function.
 */
static int libfilter(const struct dirent * entry) {
    const char *fname=entry->d_name;
    const char *s;

    s=strchr(fname, '.');
    if (s!=NULL) {
        if (strcmp(s, ".so")==0) {
            return (1);
        }
    }
    return (0);
}

tObjLib *loadlib(const char *path) {
    tObjLib *newlib=(tObjLib*)(malloc(sizeof(tObjLib)));

    if (newlib!=0) {
        newlib->next=0;
        newlib->handle=dlopen(path, RTLD_NOW|RTLD_GLOBAL);
        newlib->path=strdup(path);
        newlib->createobj=(tCreateObject)(dlsym(newlib->handle, "createobj"));
        //Log.Error("Loading shared library: %s : %s\n", path, dlerror());
    } else {

    }
    return (newlib);
}

void loadfactory(const char * path) {
    struct dirent **libs;
    char libname[PATH_MAX];
    int count;
    tObjLib *newlib;

    count=scandir(path, &libs, libfilter, alphasort);
    while (count>0) {
        count--;
        sprintf(libname, "%s/%s", path, libs[count]->d_name);
        newlib=loadlib(libname);
        /*
         * Search the last entry in newlib
         */
        tObjLib *liblist=newlib;

        while (liblist->next != 0) {
            liblist=liblist->next;
        }
        liblist->next=factorylibs;

        factorylibs=newlib;
    }
}

tSimObj *factory_createobj(typeid_t objtypeid, objectid_t objid, uint64_t droptime) {
    tObjLib *liblist=factorylibs;

    while ((liblist!=0) && (liblist->tid!=objtypeid)) {
        liblist=liblist->next;
    }
    if (liblist != 0) {
        return (liblist->createobj(objid, droptime));
    } else {

    }
    return (0);
}

tSimObj *factory_createnewobj(typeid_t objtypeid, objectid_t objid) {
    tObjLib *liblist=factorylibs;

    while ((liblist!=0) && (liblist->tid!=objtypeid)) {
        liblist=liblist->next;
    }
    if (liblist != 0) {
        return (liblist->createnewobj(objid));
    } else {

    }
    return (0);
}

tSimObj *factory_createfromtemplate(typeid_t objtypeid, templateid_t templateid, objectid_t objid) {
    tObjLib *liblist=factorylibs;

    while ((liblist!=0) && (liblist->tid!=objtypeid)) {
        liblist=liblist->next;
    }
    if (liblist != 0) {
        return (liblist->createnewfromtemplate(templateid, objid));
    } else {

    }
    return (0);
}
/*
 * First search the last entry in newlib. Maybe we have multiple factories to add.
 */
void addsimobjfactory(tObjLib *newlib) {
    tObjLib *liblist=newlib;

    while (liblist->next != 0) {
        liblist=liblist->next;
    }

    liblist->next=factorylibs;
    factorylibs=newlib;
}
