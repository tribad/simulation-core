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
