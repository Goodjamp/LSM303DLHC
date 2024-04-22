#include "FreeRTOS.h"
#include "task.h"

#include "SystemClock.h"
#include "I2c.h"
#include "ExtEv.h"
#include "DebugServices.h"
#include "Lsm303dlhc.h"


void i2cTransactionComplete(bool txSuccess);
void extEvDrdy(void);
volatile bool txCInProcess = false;
Lsm303dlhcH lsm3030glhcHandler;

I2cSettings i2cLsm303dlhcSettings = {
    .lsm303dlhc.cb = {i2cTransactionComplete},
};
ExtEvSettings extEvLsm303dlhcSettings = {
    .lsm303dlhc.extEvCb = extEvDrdy
};

void i2cTransactionComplete(bool txSuccess)
{
    if (txSuccess ==false) {
        return;
    }
    txCInProcess = false;
    lsm303dlhcI2cComplete(&lsm3030glhcHandler);
}

void extEvDrdy(void)
{
    lsm303dlhcDrdy(&lsm3030glhcHandler);
}

bool lsm3030dlhcI2cTx(uint8_t devAdd, uint8_t *data, uint8_t dataNumber)
{
    I2cResult result;

    result = i2cTx(I2C_TARGET_LSM303DLHC, devAdd, data, dataNumber);
    return result == I2C_OK;
}

bool lsm3030dlhcI2cRx(uint8_t devAdd, uint8_t txData, uint8_t *data, uint8_t dataNumber)
{
    I2cResult result;

    result = i2cRx(I2C_TARGET_LSM303DLHC, devAdd, &txData, 1, data, dataNumber);
    return result == I2C_OK;
}

#define DEVICE_ADDREEE    0x1E

void main(void)
{
    uint8_t address = 0x00;
    uint8_t rxBuff[12];

    /* Configure the system clock */
    systemClockInit();
    debugServicesInit(NULL);

    i2cInit(I2C_TARGET_LSM303DLHC, i2cLsm303dlhcSettings);
    extEvInit(EXT_EV_TARGET_LSM303DLHC, extEvLsm303dlhcSettings);
    //lsm303dlhcMInit(&lsm3030glhcHandler, lsm3030dlhcI2cTx, lsm3030dlhcI2cRx);
    uint8_t txBuff[] = {0x00, 0x07 << 2};

    txCInProcess = true;
    i2cTx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, txBuff, sizeof(txBuff));
    while (txCInProcess){}

    txBuff[0] = 2;
    txBuff[1] = 0;
    txCInProcess = true;
    i2cTx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, txBuff, sizeof(txBuff));
    while (txCInProcess){}

    for (;;){
        /*
        txCInProcess = true;
        i2cTx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, txBuff, sizeof(txBuff));
        while (txCInProcess){}
        txCInProcess = true;
        i2cRx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, &address, 1, rxBuff, 1);
        while (txCInProcess){}
        i2cRx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, &address, 1, rxBuff, 12);
        txCInProcess = true;
        while (txCInProcess){}
        */
    }
}
