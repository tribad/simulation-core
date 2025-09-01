// MIT License
//
// Copyright (c) 2011 Hans-Juergen Lange<hjl@simulated-universe.de>
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

// **************************************************************************
//
// Modul-Name        : CThread.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 16.01.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_THREADS_LINUXTHREADS_CTHREAD_INC
#define __EXTENSIONLIBS_THREADS_LINUXTHREADS_CTHREAD_INC
#include <string>
// **************************************************************************
//                             F o r w a r d s
// **************************************************************************
extern "C" {
void* startfnc(void* ptr);
} // extern C 
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CThread {
public:
    CThread();
    CThread(const std::string& aName);
    CThread(const CThread& aOld);
    virtual ~CThread();
    void Create();
    virtual bool InitInstance();
    virtual long Run();
    virtual void ExitInstance();
    void SetName(const std::string& aName);
    std::string& GetName();
    bool DoDelete();
    void Stop() {Running = false;}
public:
    static int Count;
    bool       Running;
protected:
    std::string Name;
    pthread_t ThreadID;
private:
    bool bDoDelete;
};
#endif // __EXTENSIONLIBS_THREADS_LINUXTHREADS_CTHREAD_INC
