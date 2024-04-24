#ifndef __LSM303DLHC_h__
#define __LSM303DLHC_h__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LSM303DLHC_M_RATE_0_75,
    LSM303DLHC_M_RATE_1_5,
    LSM303DLHC_M_RATE_3,
    LSM303DLHC_M_RATE_7_5,
    LSM303DLHC_M_RATE_15,
    LSM303DLHC_M_RATE_30,
    LSM303DLHC_M_RATE_75,
    LSM303DLHC_M_RATE_220,
 } Lsm303dlhcMRate;

 typedef enum {
    LSM303DLHC_M_GAIN_0,
    LSM303DLHC_M_GAIN_1,
    LSM303DLHC_M_GAIN_2,
    LSM303DLHC_M_GAIN_3,
    LSM303DLHC_M_GAIN_4,
    LSM303DLHC_M_GAIN_5,
    LSM303DLHC_M_GAIN_6,
 } Lsm303dlhcMGain;

typedef enum {
    LSM303DLHC_STATUS_OK,
    LSM303DLHC_STATUS_HANDLER_NULL_ERROR,
    LSM303DLHC_STATUS_CB_NULL_ERROR,
    LSM303DLHC_STATUS_CB_ERROR, // Any of CB function return *false*
    LSM303DLHC_STATUS_OUT_OF_LINE_ERROR, // In case of succsesfull communication, but verify data is wrong
    LSM303DLHC_STATUS_GAIN_ERROR,
    LSM303DLHC_STATUS_RATE_ERROR,
    LSM303DLHC_STATUS_VERIFY_ERROR,
    LSM303DLHC_STATUS_BUSSY_ERROR,
    LSM303DLHC_STATUS_TIMEOUTE_ERROR, // The I2C communication timeout occurred
} Lsm303dlhcStatus;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} Lsm303dlhcMagnetic;

typedef bool (*I2cTxCb)(uint8_t devAdd, uint8_t *data, uint8_t dataNumber);
typedef bool (*I2cRxCb)(uint8_t devAdd, uint8_t *txData, uint8_t *data, uint8_t dataNumber);
typedef void (*Lsm303dlhcMMesCompleteCb)(Lsm303dlhcMagnetic rawMagnetic, uint16_t angle);

typedef struct Lsm303dlhcS * Lsm303dlhcHandler;

/**
 * @brief Init the magnetic measurements of the lsm303dlhc sensor
 */
Lsm303dlhcHandler lsm303dlhcMInit(I2cTxCb txCb, I2cRxCb rxCb);
Lsm303dlhcStatus lsm303dlhcIsConnected(Lsm303dlhcHandler handler, bool *isConnect);
Lsm303dlhcStatus lsm303dlhcMSetRate(Lsm303dlhcHandler handler, Lsm303dlhcMRate rate);
Lsm303dlhcStatus lsm303dlhcMSetGain(Lsm303dlhcHandler handler, Lsm303dlhcMGain gain);
Lsm303dlhcStatus lsm303dlhcMesMBlocking(Lsm303dlhcHandler handler, Lsm303dlhcMagnetic *rawMagnetic, uint16_t angle);
Lsm303dlhcStatus lsm303dlhcMesM(Lsm303dlhcHandler handler, Lsm303dlhcMMesCompleteCb mesCompleteCb);
Lsm303dlhcStatus lsm303dlhcMesMStart(Lsm303dlhcHandler handler, Lsm303dlhcMMesCompleteCb mesCompleteCb);
Lsm303dlhcStatus lsm303dlhcMesMStop(Lsm303dlhcHandler handler);

/**
 * @brief User call from the DRDY interrupt
 */
void lsm303dlhcDrdy(Lsm303dlhcHandler handler);

/**
 * @brief User call after complete I2C transaction (Tx or Rx)
 */
void lsm303dlhcI2cComplete(Lsm303dlhcHandler handler);

#endif