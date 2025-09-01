// MIT License
//
// Copyright (c) 2018 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#ifndef MAINLOADSIMULATION_H
#define MAINLOADSIMULATION_H


class CMainLoadSimulation : public CMainCtrl
{
public:
    CMainLoadSimulation() : CMainCtrl()  {substate = 0;}
    virtual ~CMainLoadSimulation() {}
    virtual bool Action();
    virtual CMainCtrl::State Process(tSigSQLExecStringReplyPtr msg);
    virtual CMainCtrl::State Process(tSigSQLFetchReplyPtr aMsg);
private:
    void LoadObjects(tSQLRecordSet* aData);
    CMainCtrl::State  LoadData(tSQLRecordSet* aData);
private:
    int substate;
    int columncount;
};

#endif // MAINLOADSIMULATION_H
