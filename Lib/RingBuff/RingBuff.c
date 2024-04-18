#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RingBuff.h"

uint32_t ringBuffInit(RingBuffH *ringBuff, uint8_t *buff,
                      uint32_t size, uint32_t depth,
                      BlockAtomic blockAtomic)
{
    if (buff == NULL) {
        return RING_BUFF_BUFF_ERROR;
    }

    if (ringBuff == NULL) {
        return RING_BUFF_HANDLER_ERROR;
    }

    ringBuff->blockAtomic = (blockAtomic) ? (blockAtomic) : NULL;
    ringBuff->buff = buff;
    ringBuff->readP = 0;
    ringBuff->writeP = 0;
    ringBuff->depth = depth;
    ringBuff->size = size;
    ringBuff->cnt = 0;

    return RING_BUFF_OK;
}

uint8_t ringBuffGetCnt(RingBuffH *ringBuff)
{
    if (!ringBuff) {
        return 0;
    }
    return ringBuff->cnt;
}

int32_t ringBuffClear(RingBuffH *ringBuff)
{
    if (!ringBuff) {
        return RING_BUFF_HANDLER_ERROR;
    }
    if (ringBuff->blockAtomic) {
        ringBuff->blockAtomic(true);
    }
    ringBuff->writeP = 0;
    ringBuff->readP = 0;
    ringBuff->cnt = 0;
    if (ringBuff->blockAtomic) {
        ringBuff->blockAtomic(false);
    }
    return RING_BUFF_OK;
}

int32_t ringBuffPush(RingBuffH *ringBuff, const uint8_t buff[], RingBuffSizeT size)
{
    if (!ringBuff) {
        return RING_BUFF_HANDLER_ERROR;
    }
    if (ringBuff->blockAtomic) {
        ringBuff->blockAtomic(true);
    }
    if (ringBuff->cnt == ringBuff->depth) {
        if (ringBuff->blockAtomic) {
            ringBuff->blockAtomic(false);
        }
        return RING_BUFF_FULL;
    }
    if (size > ringBuff->size) {
        if (ringBuff->blockAtomic) {
            ringBuff->blockAtomic(false);
        }
        return RING_BUFF_SIZE_ERROR;
    }
    uint32_t bufInsSize = ringBuff->size + sizeof(RingBuffSizeT);
    uint8_t(*tempBuff)[bufInsSize] = (uint8_t(*)[bufInsSize])ringBuff->buff;
    memcpy(tempBuff[ringBuff->writeP], &size, sizeof(RingBuffSizeT));
    memcpy(&tempBuff[ringBuff->writeP][sizeof(RingBuffSizeT)], buff, size);
    if (++ringBuff->writeP >= ringBuff->depth) {
        ringBuff->writeP = 0;
    }
    ringBuff->cnt++;
    if (ringBuff->blockAtomic) {
        ringBuff->blockAtomic(false);
    }
    return RING_BUFF_OK;
}

int32_t ringBuffPop(RingBuffH *ringBuff, uint8_t buff[], RingBuffSizeT *size)
{
    if (!ringBuff) {
        return RING_BUFF_HANDLER_ERROR;
    }
    if (ringBuff->blockAtomic) {
        ringBuff->blockAtomic(true);
    }
    if (ringBuff->cnt == 0) {
        if (ringBuff->blockAtomic) {
            ringBuff->blockAtomic(false);
        }
        return RING_BUFF_EMPTY;
    }
    uint32_t bufInsSize = ringBuff->size + sizeof(RingBuffSizeT);
    uint8_t(*tempBuff)[bufInsSize] = (uint8_t(*)[bufInsSize])ringBuff->buff;
    memcpy(size, tempBuff[ringBuff->readP], sizeof(RingBuffSizeT));
    memcpy(buff, &tempBuff[ringBuff->readP][sizeof(RingBuffSizeT)],
           *size);
    if (++ringBuff->readP >= ringBuff->depth) {
        ringBuff->readP = 0;
    }
    ringBuff->cnt--;
    if (ringBuff->blockAtomic) {
        ringBuff->blockAtomic(false);
    }
    return RING_BUFF_OK;
}
