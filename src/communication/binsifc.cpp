#include <stdint.h>
#include <stdlib.h>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "ifcthread.h"
#include "sslifcthread.h"
#include "binsifc.h"

CBinSIfc::CBinSIfc() : CSSLIfcThread(ModulId::BinSSLServer, "Binary-SSL Ifc")
{

}
