#include "FreeRTOS.h"
#include "task.h"

#include "SystemClock.h"
#include "I2c.h"
#include "DebugServices.h"

void i2cTransactionComplete(bool txSuccess);
volatile bool txCInProcess = false;

const I2cSettings i2cLsm303dlhcSettings = {
    .lsm303dlhc.cb = { i2cTransactionComplete},
};

void i2cTransactionComplete(bool txSuccess)
{
    if (txSuccess ==false) {
        return;
    }
    txCInProcess = false;
}

void delay(uint32_t cnt)
{
    while(cnt--){}
}

void main(void)
{
    /* Configure the system clock */
    systemClockInit();
    debugServicesInit(NULL);

    i2cInit(I2C_TARGET_LSM303DLHC, i2cLsm303dlhcSettings);
    uint8_t txBuff[] = {0x02, 0x03};

    for (;;){
        txCInProcess = true;
        i2cTx(I2C_TARGET_LSM303DLHC, 0x1E, txBuff, sizeof(txBuff));
        while (txCInProcess){}
        //delay(1000);
    }
}

