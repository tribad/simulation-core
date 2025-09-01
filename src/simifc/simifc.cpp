// **************************************************************************
//
//  Modul-Name     : simifc.cpp
//  Author(s)      : Hans-Juergen Lange
//
//  Copyrights 2015 by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <list>
#include <pthread.h>
#include <stdint.h>
#include <malloc.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sstream>
#include <set>
/*
 * Here starts the list of simulation headers
 */
#include <helper.h>
#include <simifc.h>
#include <coreifc.h>
/*
 * Some quick-hack debug flags.
 */
extern bool debugweb;
extern bool debugws;

extern "C" {

CSimIfc* createsimlibifc(CCoreIfc* aCore) {
    //
    //  We do save the pointer to the core interface
    __coreIfc=aCore;
    //
    //  This call gets into the implementation of the simulation.
    //  If this function is not available at link-time no simulation will be produced.
    //  It returns a new CSimIfc based object.
    return (siminit());
}
}

