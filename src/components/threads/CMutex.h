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
// Modul-Name        : CMutex.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 18.02.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_THREADS_LINUXTHREADS_CMUTEX_INC
#define __EXTENSIONLIBS_THREADS_LINUXTHREADS_CMUTEX_INC
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CMutex {
public:
    CMutex();
    ~CMutex();
    void Lock();
    void Unlock();
    bool Check();
protected:
    pthread_mutex_t mutex;
};
#endif // __EXTENSIONLIBS_THREADS_LINUXTHREADS_CMUTEX_INC
