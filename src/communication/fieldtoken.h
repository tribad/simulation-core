/*
 * tUdpGenericFrame.h
 *
 *  Created on: 14 Feb 2021
 *      Author: ltoo
 */

#ifndef FIELDTOKEN_H_
#define FIELDTOKEN_H_

#include <cstdint>

using tTokenType = uint16_t;

template <typename T>
    struct tFieldToken {
        tFieldToken() {}
        static const size_t      mSize  = sizeof(T);
        tTokenType               mToken = 0ul;
        T                        mValue = {0};
        tFieldToken(tTokenType tt, T v) {mToken = tt; mValue = v;}
        tFieldToken(tTokenType tt) {mToken = tt;}
        tFieldToken& operator=(const T& value) {mValue = value; return *this;}
    };





#endif /* FIELDTOKEN_H_ */
