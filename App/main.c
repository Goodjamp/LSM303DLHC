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
Lsm303dlhcHandler lsm3030glhcHandler;
volatile bool isInit = false;

I2cSettings i2cLsm303dlhcSettings = {
    .lsm303dlhc.cb = {i2cTransactionComplete},
};
ExtEvSettings extEvLsm303dlhcSettings = {
    .lsm303dlhc.extEvCb = extEvDrdy
};

void i2cTransactionComplete(bool txSuccess)
{
    if (txSuccess == false) {
        return;
    }
    txCInProcess = false;
    lsm303dlhcI2cComplete(lsm3030glhcHandler);
}

void extEvDrdy(void)
{
    if (isInit == false) {
        return;
    }
    lsm303dlhcDrdy(lsm3030glhcHandler);
}

bool lsm3030dlhcI2cTx(uint8_t devAdd, uint8_t *data, uint8_t dataNumber)
{
    I2cResult result;

    result = i2cTx(I2C_TARGET_LSM303DLHC, devAdd, data, dataNumber);
    return result == I2C_OK;
}

bool lsm3030dlhcI2cRx(uint8_t devAdd, uint8_t *txData, uint8_t *data, uint8_t dataNumber)
{
    I2cResult result;

    result = i2cRx(I2C_TARGET_LSM303DLHC, devAdd, txData, 1, data, dataNumber);
    return result == I2C_OK;
}

void lsm303dlhcMMesCompleteCb(Lsm303dlhcMagnetic rawMagnetic, uint16_t angle)
{
    static volatile int16_t mX;
    static volatile int16_t mY;
    static volatile int16_t mZ;

    debugServicesPinSet(DebugPin1);
    mX = rawMagnetic.x;
    mY = rawMagnetic.y;
    mZ = rawMagnetic.z;
    debugServicesPinClear(DebugPin1);
}

/*
void testI2C(void)
{
#define DEVICE_ADDREEE    0x1E
    uint8_t addressReg;
    uint8_t txBuff[] = {0x00, 0x07 << 2};
    uint8_t rxBuff[12];

    txCInProcess = true;
    i2cTx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, txBuff, sizeof(txBuff));
    while (txCInProcess){}

    txBuff[0] = 2;
    txBuff[1] = 0;
    txCInProcess = true;
    i2cTx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, txBuff, sizeof(txBuff));
    while (txCInProcess){}

    for (;;){
        txCInProcess = true;
        i2cTx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, txBuff, sizeof(txBuff));
        while (txCInProcess){}

        txCInProcess = true;
        addressReg = 0;
        i2cRx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, &addressReg, 1, rxBuff, 1);
        while (txCInProcess){}

        txCInProcess = true;
        addressReg = 0;
        i2cRx(I2C_TARGET_LSM303DLHC, DEVICE_ADDREEE, &addressReg, 1, rxBuff, 12);
        while (txCInProcess){}
    }
}
*/

bool initAndRunSensor(void)
{
    Lsm303dlhcStatus sensorResult;
    I2cResult i2cInitResult;
    ExtEvResult extInitEvResult;

    i2cInitResult = i2cInit(I2C_TARGET_LSM303DLHC, i2cLsm303dlhcSettings);
    if (i2cInitResult != I2C_OK){
        return false;
    }

    extInitEvResult = extEvInit(EXT_EV_TARGET_LSM303DLHC, extEvLsm303dlhcSettings);
    if (extInitEvResult != EXT_EV_OK){
        return false;
    }

    volatile uint32_t cnt = 40000;
    while(cnt--){}

    lsm3030glhcHandler = lsm303dlhcMInit(lsm3030dlhcI2cTx, lsm3030dlhcI2cRx);
    if (lsm3030glhcHandler == NULL) {
        return false;
    }
    sensorResult = lsm303dlhcMSetRate(lsm3030glhcHandler, LSM303DLHC_M_RATE_30);
    if (sensorResult != LSM303DLHC_STATUS_OK) {
        return false;
    }
    sensorResult = lsm303dlhcMSetGain(lsm3030glhcHandler, LSM303DLHC_M_GAIN_0);
    if (sensorResult != LSM303DLHC_STATUS_OK) {
        return false;
    }

    sensorResult = lsm303dlhcMesMStart(lsm3030glhcHandler, lsm303dlhcMMesCompleteCb);
    if (sensorResult != LSM303DLHC_STATUS_OK) {
        return false;
    }

    return true;
}

void main(void)
{
    /* Configure the system clock */
    systemClockInit();
    debugServicesInit(NULL);
    initAndRunSensor();

    isInit = true;
    for (;;){
    }
}
