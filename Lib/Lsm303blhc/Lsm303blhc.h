#ifndef __LSM303DLHC_h__
#define __LSM303DLHC_h__

#include <stdint.h>

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} Lsm303ldhcMagnetic;

void lsm303ldhcMInit(void);
void lsm303ldhcMSetRate(void);
void lsm303ldhcMSetImplifier(void);
void lsm303ldhcMGet(Lsm303ldhcMagnetic *magnetic);

#endif