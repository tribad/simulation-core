// MIT License
//
// Copyright (c) 2021 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#ifndef TUDPFRAME_H_
#define TUDPFRAME_H_

#include <sys/socket.h>
#include <netinet/in.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <fieldtoken.h>

#define UDPFRAMESIZE 1024

class tUdpFrame {
public:
    tUdpFrame() {
    }
    tUdpFrame(uint64_t aConnection, uint64_t aTick, const struct sockaddr_in& aAddress = sockaddr_in()) {
        *this << aConnection << aTick;
        mTick    = aTick;
        mHash    = aConnection;
        mAddress = aAddress;
    }
    tUdpFrame& operator << (double value) {
        expand(sizeof(value));
        memcpy(mLast->mBuffer + mLast->mIov->iov_len, &value, sizeof(value));
        mLast->mIov->iov_len += sizeof(value);

        return *this;
    }
    tUdpFrame& operator << (uint64_t value) {
        expand(sizeof(value));
        memcpy(mLast->mBuffer + mLast->mIov->iov_len, &value, sizeof(value));
        mLast->mIov->iov_len += sizeof(value);

        return *this;
    }
    template <typename T>
        tUdpFrame& operator<<(const tFieldToken<T>& token) {
            size_t need = token.mSize + sizeof(tTokenType);

            expand(need);
            memcpy(mLast->mBuffer + mLast->mIov->iov_len, &(token.mToken), sizeof(tTokenType));
            mLast->mIov->iov_len += sizeof(tTokenType);

            memcpy(mLast->mBuffer + mLast->mIov->iov_len, &(token.mValue), token.mSize);
            mLast->mIov->iov_len += token.mSize;

            return *this;
        }
private:
    void expand(size_t aSize) {
        if (mLast->mIov->iov_len + aSize > UDPFRAMESIZE) {
            tUdpFrame* newFrame = new tUdpFrame(mHash, mTick, mAddress);
            //
            //  Anyway the last next gets the new frame.
            mLast->mNext = newFrame;
            //
            //  Fix any last pointers to prevent errors in data.
            tUdpFrame*  f = this;
            while (f != nullptr) {
                f->mLast = newFrame;
                f = f->mNext;
            }
        }
    }
public:
	char               mBuffer[UDPFRAMESIZE] = {0};
	tUdpFrame*         mLast                 = this;
	tUdpFrame*         mNext                 = nullptr;
	uint64_t           mTick                 = 0u;
	uint64_t           mHash                 = 0u;
	struct sockaddr_in mAddress;
	struct iovec       mIov[1]               = { mBuffer, 0ul};
	struct msghdr      mMsg                  = { &mAddress, static_cast<socklen_t>(sizeof(mAddress)), mIov, 1, nullptr, 0, 0 };
};



#endif /* TUDPFRAME_H_ */
