#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    I2C_OK,
    I2C_TARGET_ERROR,
    I2C_BUFF_NULL_ERROR,
    I2C_DATA_SIZE_0_ERROR,
    I2C_BUSSY,
} I2cResult;

typedef enum {
    I2C_TARGET_LSM303DLHC,
} I2cTarget;

typedef struct {
    void (*transactionComplete)(bool txSuccess);
} I2cLsm303dlhcCb;

typedef struct {
    I2cLsm303dlhcCb cb;
} Lsm303dlhcSettings;

typedef union {
    Lsm303dlhcSettings lsm303dlhc;
} I2cSettings;

I2cResult i2cInit(I2cTarget i2cTarget, I2cSettings settings);
I2cResult i2cTx(I2cTarget i2cTarget, uint8_t devAddr, uint8_t *buff, uint32_t size);
I2cResult i2cRx(I2cTarget i2cTarget, uint8_t devAddr, uint8_t regAddr, uint8_t *buff, uint32_t size);

#endif