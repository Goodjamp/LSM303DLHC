#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    I2C_OK,
    I2C_TARGET_ERROR,
    I2C_BUFF_NULL_ERROR,
    I2C_DATA_SIZE_0_ERROR,
} I2cResult;

typedef enum {
    I2C_TARGET_SSD1306,
} I2cTarget;

typedef struct {
    void (*txComplete)(bool txSuccess);
} I2cSsd1306Cb;

typedef struct {
    uint8_t dummy;
} Ssd1306Settings;

typedef union {
    I2cSsd1306Cb ssd1306;
} I2cCb;

typedef union {
    Ssd1306Settings ssd1306;
} I2cSettings;

I2cResult i2cInit(I2cTarget i2cTarget, I2cSettings settings, I2cCb cb);
I2cResult i2cTx(I2cTarget i2cTarget, uint8_t *buff, uint32_t size);

#endif