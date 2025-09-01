// **************************************************************************
//
// Modul-Name        : CMicroThread.cpp
// Author            : 
// Creation-Date     : 19.04.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
// **************************************************************************
//                   E x t r a   I n c l u d e   L i s t
// **************************************************************************
#include <stddef.h>
#include <stdint.h>
#include <memory.h>
#include "CMicroThread.h"
// **************************************************************************
//
//  Method-Name       : CMicroThread()
//  Author            : 
//  Creation-Date     : 19.04.2011
//  Modification-Date : 24.04.2011 10:37:35
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CMicroThread::CMicroThread() {
    bRunning=false;
}
// **************************************************************************
//
//  Method-Name       : Start()
//  Author            : 
//  Creation-Date     : 19.04.2011
//  Modification-Date : 24.04.2011 10:37:34
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CMicroThread::Start() {
    bRunning=true;
}
// **************************************************************************
//
//  Method-Name       : Run()
//  Author            : 
//  Creation-Date     : 19.04.2011
//  Modification-Date : 24.04.2011 10:37:54
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CMicroThread::Run() {
    
}
// **************************************************************************
//
//  Method-Name       : Terminate()
//  Author            : 
//  Creation-Date     : 19.04.2011
//  Modification-Date : 24.04.2011 10:37:49
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CMicroThread::Terminate() {
    bRunning=false;
}
