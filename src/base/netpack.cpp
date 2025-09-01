#include "netpack.h"
uint64_t tNetPack::counter            = 0;
pthread_mutex_t tNetPack::counterlock = PTHREAD_MUTEX_INITIALIZER;
