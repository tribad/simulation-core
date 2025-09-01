#ifndef __WSMESSAGE_INC__
#define __WSMESSAGE_INC__

struct tWSMessage {
    uint8_t  state;
    uint8_t  hdrbytecount;
    uint64_t recv;
    bool     fin;
    bool     rsv1;
    bool     rsv2;
    bool     rsv3;
    uint8_t  opcode;
    bool     mask;
    uint64_t payloadlen;
    uint8_t  maskkey[4];
};


#endif
