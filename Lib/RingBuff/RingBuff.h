#ifndef _RING_BUFF_H__
#define _RING_BUFF_H__

#include <stdint.h>
#include <stdbool.h>

#define RING_BUFF_OK               0
#define RING_BUFF_EMPTY            1
#define RING_BUFF_FULL             2
#define RING_BUFF_BUFF_ERROR       -1
#define RING_BUFF_HANDLER_ERROR    -2
#define RING_BUFF_SIZE_ERROR       -3

typedef uint8_t RingBuffSizeT;

#define RING_BUFF_CREATE_BUFF(name, size, depth) \
    static uint8_t name[depth * (size + sizeof(RingBuffSizeT))];

typedef void (*BlockAtomic)(bool block);

typedef struct {
    uint8_t *buff;
    uint8_t writeP;
    uint8_t readP;
    uint8_t cnt;
    uint32_t depth;
    RingBuffSizeT size;
    BlockAtomic blockAtomic;
} RingBuffH;

uint32_t ringBuffInit(RingBuffH *ringBuff, uint8_t *buff,
                      uint32_t size, uint32_t depth,
                      BlockAtomic blockAtomic);
uint8_t ringBuffGetCnt(RingBuffH *ringBuff);
int32_t ringBuffPush(RingBuffH *ringBuff, const uint8_t buff[], RingBuffSizeT size);
int32_t ringBuffPop(RingBuffH *ringBuff, uint8_t buff[], RingBuffSizeT *size);
int32_t ringBuffClear(RingBuffH *ringBuff);

#endif
