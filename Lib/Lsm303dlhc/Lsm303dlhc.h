#ifndef __LSM303DLHC_h__
#define __LSM303DLHC_h__

#include <stdint.h>
#include <stdbool.h>

typedef bool (*I2cTxCb)(uint8_t devAdd, uint8_t regAdd, uint8_t regNumber, uint8_t *txData);
typedef bool (*I2cRxCb)(uint8_t devAdd, uint8_t regAdd, uint8_t regNumber, uint8_t *rxData);
typedef bool (*Lsm303dlhcMMesCompleteCb)(uint8_t Lsm303dlhcMagnetic);

typedef struct {
    I2cTxCb txCb;
    I2cRxCb rxCb;
    bool bussy;
} Lsm303dlhcH;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} Lsm303dlhcMagnetic;

typedef enum {
    LSM303DLHC_M_RATE_0_75,
    LSM303DLHC_M_RATE_1_5,
    LSM303DLHC_M_RATE_3_0,
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
    LSM303DLHC_OUT_OF_LINE_ERROR, // In case of succsesfull communication, but verify data is wrong
    LSM303DLHC_GAIN_ERROR,
    LSM303DLHC_RATE_ERROR,
    LSM303DLHC_VERIFY_ERROR,
    LSM303DLHC_BUSSY_ERROR,
    LSM303DLHC_TIMEOUTE_ERROR, // The I2C communication timeout occurred
} Lsm303dlhcStatus;

/**
 * @brief Init the magnetic measurements part of the lsm303dhlc sensor
 */
Lsm303dlhcStatus lsm303dlhcMInit(Lsm303dlhcH *handler, I2cTxCb txCb, I2cRxCb rxCb);
Lsm303dlhcStatus lsm303dlhcMSetRate(Lsm303dlhcH *handler, Lsm303dlhcMRate rate);
Lsm303dlhcStatus lsm303dlhcMSetGain(Lsm303dlhcH *handler, Lsm303dlhcMGain gain);
Lsm303dlhcStatus lsm303dlhcMesMBlocking(Lsm303dlhcH *handler, Lsm303dlhcMagnetic *magnetic);
Lsm303dlhcStatus lsm303dlhcMesM(Lsm303dlhcH *handler, Lsm303dlhcMMesCompleteCb *mesCompleteCb);
Lsm303dlhcStatus lsm303dlhcMesMStart(Lsm303dlhcH *handler, Lsm303dlhcMMesCompleteCb *mesCompleteCb);

/**
 * @brief User call from the DRDY interrupt
 */
Lsm303dlhcStatus lsm303dlhcDrdy(Lsm303dlhcH *handler);
Lsm303dlhcStatus lsm303dlhcI2cComplete(Lsm303dlhcH *handler);

#endif