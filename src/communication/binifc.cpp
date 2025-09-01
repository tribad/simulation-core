#include <stdint.h>
#include <stdlib.h>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "ifcthread.h"
#include "binifc.h"

CBinIfc::CBinIfc() : CIfcThread(ModulId::BinIfcServer, "Binary-Ifc")
{

}
