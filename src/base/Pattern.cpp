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
//  Modul-Name        : pattern.cpp
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
// **************************************************************************
//                   E x t r a   I n c l u d e   L i s t
// **************************************************************************
#include <string.h>
#include <string>
#include <stddef.h>
#include <stdint.h>
// **************************************************************************
//                   F i r s t   I n c l u d e   L i s t
// **************************************************************************
#include "Pattern.h"
// **************************************************************************
//                   L a s t   I n c l u d e   L i s t
// **************************************************************************
// **************************************************************************
//                   N a m e s p a c e   L i s t
// **************************************************************************
using namespace std;
// **************************************************************************
//
//  Method-Name       : PatternMatch()
//  Author            : 
//  Creation-Date     : 09.04.2011
//  Modification-Date : 10.04.2011 07:28:19
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool PatternMatch(const std::string& aPattern, const std::string& aString) {
    return (PatternMatch(aPattern.c_str(), aString.c_str()));
}
// **************************************************************************
//
//  Method-Name       : PatternMatch()
//  Author            : 
//  Creation-Date     : 09.04.2011
//  Modification-Date : 10.04.2011 07:28:25
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool PatternMatch(const char* aPattern, const std::string& aString) {
    return (PatternMatch(aPattern, aString.c_str()));
}
// **************************************************************************
//
//  Method-Name       : PatternMatch()
//  Author            : 
//  Creation-Date     : 09.04.2011
//  Modification-Date : 10.04.2011 07:28:28
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool PatternMatch(const std::string& aPattern, const char* aString) {
    return (PatternMatch(aPattern.c_str(), aString));
}
// **************************************************************************
//
//  Method-Name       : PatternMatch()
//  Author            : 
//  Creation-Date     : 09.04.2011
//  Modification-Date : 12.04.2011 10:26:29
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool PatternMatch(const char* aPattern, const char* aString) {
    int  ppos=0;
    int  spos=0;
    bool missed=false;
    //
    //  Fast exit from compare.
    if (strcmp(aPattern, "*")==0) {
        return (true);
    } else {
        //
        //  here we do a more complex compare.
        do {
            switch (aPattern[ppos]) {
            case '*':
                if (aPattern[ppos+1]==aString[spos]) {
                    ppos+=2;
                } else {
                }
                break;
            case '?':
                ppos++;
                break;
            default:
                if (aPattern[ppos]!=aString[spos]) {
                    missed=true;
                } else {
                    ppos++;
                }
                break;
            }
            spos++;
        } while ((!missed) && ((aPattern[ppos]!='\0') && (aString[spos]!='\0')));
        if ((aPattern[ppos]=='\0') && (aString[spos]=='\0')) {
            return (true);
        }
        if ((aPattern[ppos] != '\0') && (aPattern[ppos] != '*')){
            return (false);
        }
    }
    return (!missed);
}
