// **************************************************************************
//
// Modul-Name        : cacheentry
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 27.01.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
//
//  System Headers.
#include <gnutls/gnutls.h>
#include <gnutls/crypto.h>
//
//  C++ headers
#include <cstring>
//
//  Modul header.
#include "cacheentry.h"
//
//  Simulated-Universe headers.
#include <CFile.h>

#define HASH_ALGO GNUTLS_DIG_SHA512

CCacheEntry::CCacheEntry()
{
    Id     = 0;
    Buffer = nullptr;
    Hash   = new uint8_t[gnutls_hash_get_len(HASH_ALGO)];
    next   = prev = nullptr;
}

CCacheEntry::CCacheEntry(int aId, const CFileInfo& aInfo) : CCacheEntry()
{
    Id   = aId;
    Info = aInfo;

    InternalLoad();
}

CCacheEntry::~CCacheEntry()
{
    if (Buffer != nullptr) {
        delete [] Buffer;
    }
    if (Hash != nullptr) {
        delete [] Hash;
    }
#if _DEBUG
    Hash   = nullptr;
    Buffer = nullptr;
    Size   = 0;
    Id     = 0;
#endif
}

void CCacheEntry::Load() {
    if (LoadIgnore == false) {
        InternalLoad();
    } else {
        LoadIgnore = false;
    }
}

void CCacheEntry::InternalLoad() {
    CFile f(Info.GetName());
    //
    //  Processing only files and check if we should ignore the load request.
    if (!Info.IsDirectory()) {

        f.Open(O_RDONLY);
        if (f.IsOpen()) {
            LoadIgnore = true;
            Lock.lock();

            if (Info.GetSize() != f.GetInfo().GetSize()) {
                if (Buffer != nullptr) {
                    delete [] Buffer;
                    Buffer = nullptr;
                }
            }

            Info = f.GetInfo();
            if (Buffer == nullptr) {
                Buffer = new uint8_t[Info.GetSize()];
            }
            f.Read(Buffer, Info.GetSize());
            //
            //  Calc the hash
            int              hasherr  = 0;
            gnutls_hash_hd_t hash_hdl;

            hasherr = gnutls_hash_init(&hash_hdl, HASH_ALGO);
            if (hasherr == 0) {
                hasherr = gnutls_hash(hash_hdl, Buffer, Info.GetSize());
                if (hasherr == 0) {
                    gnutls_hash_deinit(hash_hdl, Hash);
                }
            }
            if (hasherr != 0) {
                memset(Hash, 0, gnutls_hash_get_len(HASH_ALGO));
            }
            Lock.unlock();
        }
    }
}

void CCacheEntry::MoveUp() {

}

bool CCacheEntry::operator ==(const std::shared_ptr<CCacheEntry>& other) {
    bool retval = true;

    retval = (memcmp(Hash, other->Hash, gnutls_hash_get_len(HASH_ALGO)) == 0);
    return retval;
}
