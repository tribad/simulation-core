// MIT License
//
// Copyright (c) 2015 Hans-Juergen Lange<hjl@simulated-universe.de>
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

