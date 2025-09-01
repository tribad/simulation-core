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

#ifndef LOGGER_H
#define LOGGER_H

#include <cstdint>
#include <string>
#include <sstream>

#define MAX_LOGMESSAGE  (16384u)
/*
 * This is the bitposition for the modul-flag.
 * For now this is limited to 64 moduls
 */
enum class ModulId : uint8_t {
    Main,
    Simulation,
    SimWorker,
    HttpIfcServer,
    HttpSSLServer,
    PhpIfcServer,
    JsonIfcServer,
    PhpSSLServer,
    WsIfcServer,
    WsSSLServer,
    BinIfcServer,
    BinSSLServer,
    CommThread,
    MariaDB,
    PostgreSQL,
    Infiniband,
    SimulationLib,
    ContentLib,
    Cache,
    UdpIfc
};

enum class LogLevel : uint64_t {
    Unbreakable,
    Critical,
    Error,
    Warn,
    Info,
    Debug,
    Trace
};

class CLogger
{
public:
    CLogger(ModulId aModul, std::string aName);
    ~CLogger();
    void SetLogLevel(LogLevel aLevel);
    void Enable(ModulId aModul);
    void Disable(ModulId aModul);
    int Write(LogLevel aLevel, const char* aFormat,  ...) ;
    int Write(LogLevel aLevel, const std::string& aMsg);
    int Write(LogLevel aLevel, const std::ostringstream&);

private:
    char     buffer[MAX_LOGMESSAGE];
    uint64_t modul;
    uint64_t level;
};

#endif // LOGGER_H
