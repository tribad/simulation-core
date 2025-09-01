// **************************************************************************
//
//  Modul-Name     : simidmap.cpp
//  Author(s)      : Hans-Juergen Lange
//
//  Copyrights 2014 by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <memory.h>
#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <simidmap.h>
/*
 *There is only one idmap in a simulation.
 */
static tSimIdMap idmap;
static pthread_mutex_t access;
/*
 * The ids are split into ranges. These ranges must be initialized.
 */
static void init_range(tIdMapRange *range, uint64_t *aStart, const uint32_t aRangeOffset, const uint32_t aRange, const uint32_t aMoveAhead) {
    range->Start=aStart;
    range->Range=aRange;
    range->FreeCount=aRange*64ul;
    range->RangeOffset=aRangeOffset;
    range->MoveAheadRange=aMoveAhead;
    range->Next=0;
    range->StartSearch=0;
    //
    //  Mark the id 0 as used. This is to be able to use 0 as an Invalid Id.
    if (aRangeOffset==0) {
        range->Start[0]&=(~1ul);;
        range->FreeCount--;
    } else {
    }

}

static uint32_t allocate_range(tIdMapRange *range) {
    uint32_t retval;
    //
    //  Search an entry with not all bits set.
    //  StartSearch may point somewhere into the range. But not necessarily to a value
    //  with one clear bit.
    while ((!range->Start[range->StartSearch]) && (range->StartSearch < range->Range)) {
        //
        //  MoveOn
        range->StartSearch++;
    }
    //
    //   Check whether we are in range.
    if (range->StartSearch < range->Range) {
        //
        //  if there is a single 1 bit we get the index+1 to it.
        int pos=__builtin_ffsl(range->Start[range->StartSearch]);
        if (pos != 0) {
            pos--;
            //
            //  Clear the bit
            range->Start[range->StartSearch]&=(~1ul<<pos);
            //
            //  Calculate the return value.
            retval=((range->StartSearch+range->RangeOffset)*64u)+pos;
            range->FreeCount--;
        } else {
            retval=0;
        }
    } else {
        retval=0;
    }
    //
    //  Returning the id
    return (retval);
}

static void free_range(tIdMapRange *range, uint64_t id) {
    uint32_t index=id>>6u;
    //
    //  We are getting a relative id. This is bit number within our range.
    range->Start[index]|=(1ul<<(id&0x0000003f));
    range->FreeCount++;
    //
    //  If we free an id before the actual search position we move it there.
    if (index < range->StartSearch) {
        range->StartSearch=index;
    } else {
    }
}

static void setinuse_range(tIdMapRange *range, uint64_t id) {
    uint32_t index=id>>6u;
    //
    //  We are getting a relative id. This is bit number within our range.
    range->Start[index]&=~((1ul<<(id&0x0000003f)));
    if(range->FreeCount>0) {
        range->FreeCount--;
    } else {
    }
}

void init_simidmap(int aBits, int aRangeBits, const uint32_t aMoveAhead) {
    uint32_t i;
    //
    //  Init mutex for parallel access sync
    pthread_mutex_init(&access, 0);
    //
    //  No ranges at all.
    idmap.FreeStart=idmap.FreeEnd=0;
    //
    //  Create the map and clear it.
    idmap.BitMapSize=(1ul<<aBits)/(sizeof(idmap.BitMap[0])*8u);
    idmap.BitMap=(uint64_t*)(malloc(sizeof(uint64_t)*idmap.BitMapSize));
    memset(idmap.BitMap, 0xffff, idmap.BitMapSize*sizeof(idmap.BitMap[0]));
    //
    //  The number of rangebits is the number of bits in the map that makes a range.
    //  Because of the 64bit per entry we add 6 bits.
    idmap.RangeBits=aBits-aRangeBits;
    //
    //  Now create the ranges. Using the original parameter not the RangeBits that larger by 6
    //  RangeCount is the number of ranges.
    idmap.RangeCount=(1u << aRangeBits);
    idmap.Ranges=(tIdMapRange*)malloc( sizeof(tIdMapRange)*idmap.RangeCount);
    //
    //  Initialize the ranges.
    for (i=0;i<idmap.RangeCount;++i) {
        init_range(idmap.Ranges+i, idmap.BitMap+(idmap.BitMapSize/idmap.RangeCount)*i, (idmap.BitMapSize/idmap.RangeCount)*i, (idmap.BitMapSize/idmap.RangeCount), aMoveAhead);
        //
        // If no Range is in the FreeList, Start and End point to the same Element.
        if (idmap.FreeStart==0) {
            idmap.FreeStart=idmap.FreeEnd=idmap.Ranges;
        } else {
            //
            //  New Elements are put at the end of the list.
            idmap.FreeEnd->Next=idmap.Ranges+i;
            idmap.FreeEnd=idmap.Ranges+i;
        }
    }
}

uint64_t allocate_simid(void) {
    tIdMapRange *oldstart;
    //
    //  Lock mutex
    pthread_mutex_lock(&access);
    //
    //  Check if we have any free ranges and after that if the Range has any free ids.
    //  If not set the Start to the next range.
    while ((idmap.FreeStart!=0) && (idmap.FreeStart->FreeCount==0)) {
        oldstart=idmap.FreeStart;
        idmap.FreeStart=idmap.FreeStart->Next;
        oldstart->Next=0;
    }
    //
    //  Maybe the end of the list has been reached. Check again the FreeStart Pointer.
    if (idmap.FreeStart!=0) {
        uint64_t id=allocate_range(idmap.FreeStart);
        pthread_mutex_unlock(&access);
        return (id);
    } else {
    }
    //
    //  The Id 0 is not valid.
    pthread_mutex_unlock(&access);
    return (0);
}

void free_simid(uint64_t id) {
    int rangeindex=id>>idmap.RangeBits;
    pthread_mutex_lock(&access);
    //
    //  Pass on the relative id in the range.
    free_range(idmap.Ranges+rangeindex, id&((1ul<<idmap.RangeBits)-1));
    //
    //  If the count is 1 after freeing it. We must append the range into the free list.
    if (idmap.Ranges[rangeindex].FreeCount==1) {
        idmap.FreeEnd->Next=idmap.Ranges+rangeindex;
        idmap.FreeEnd=idmap.Ranges+rangeindex;
    } else {
    }
    pthread_mutex_unlock(&access);
}

void setinuse_simid(uint64_t id) {
    int rangeindex=id>>idmap.RangeBits;
    //
    //  Pass on the relative id in the range.
    setinuse_range(idmap.Ranges+rangeindex, id&((1ul<<idmap.RangeBits)-1));
    if (idmap.Ranges[rangeindex].FreeCount==0) {
        uint32_t i;
        //
        //  search the range that points to the now full range.
        for (i=0; idmap.RangeCount; ++i) {
            if (idmap.Ranges[i].Next==idmap.Ranges+rangeindex) {
                idmap.Ranges[i].Next=idmap.Ranges[rangeindex].Next;
            }
        }
    }

}
void rebuild_rangelist(void) {
    uint32_t i;
    idmap.FreeEnd=idmap.FreeStart=0;
    for (i=0;i<idmap.RangeCount;++i) {
        idmap.Ranges[i].Next=0;
        if (idmap.Ranges[i].FreeCount>0) {
            //
            // If no Range is in the FreeList, Start and End point to the same Element.
            if (idmap.FreeStart==0) {
                idmap.FreeStart=idmap.FreeEnd=idmap.Ranges+i;
            } else {
                //
                //  New Elements are put at the end of the list.
                idmap.FreeEnd->Next=idmap.Ranges+i;
                idmap.FreeEnd=idmap.Ranges+i;
            }
        } else {
        }
    }
}

