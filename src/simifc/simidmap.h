/*
 * simidmap.h
 *
 *  Created on: Jun 21, 2014
 *      Author: ltoo
 */

#ifndef SIMIDMAP_H_
#define SIMIDMAP_H_

#include <stdint.h>

struct tIdMapRange {
    uint64_t *Start;
    uint32_t Range;
    uint32_t RangeOffset;
    uint32_t FreeCount;
    tIdMapRange *Next;
    uint32_t StartSearch;
    uint32_t MoveAheadRange;
};

struct tSimIdMap {
    int BitMapSize;
    uint64_t *BitMap;
    uint32_t RangeCount;
    int RangeBits;
    tIdMapRange *FreeStart;
    tIdMapRange *FreeEnd;
    tIdMapRange *Ranges;
};

void init_simidmap(int aBits, int aRangeBits, const uint32_t aMoveAhead);
uint64_t allocate_simid(void);
void free_simid(uint64_t id);
void setinuse_simid(uint64_t id);
void rebuild_rangelist(void);

#endif /* SIMIDMAP_H_ */
