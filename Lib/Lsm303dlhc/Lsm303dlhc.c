#include <stdint.h>
#include <stdlib.h>

#include "Lsm303dlhc.h"

#define LSM303DLHC_M_ADDRESS           0x3C

#define LSM303DLHC_VAL_IRA_REG_M       0x48
#define LSM303DLHC_VAL_IRB_REG_M       0x34
#define LSM303DLHC_VAL_IRC_REG_M       0x33
#define LSM303DLHC_VERIFY_DATA_SIZE    3

typedef enum {
    LSM303DLHC_ADD_CRA_REG_M = 0x00,
    LSM303DLHC_ADD_CRB_REG_M = 0x01,
    LSM303DLHC_ADD_MR_REG_M = 0x02,
    LSM303DLHC_ADD_OUT_X_H_M = 0x03,
    LSM303DLHC_ADD_OUT_X_L_M = 0x04,
    LSM303DLHC_ADD_OUT_Y_H_M = 0x05,
    LSM303DLHC_ADD_OUT_Y_L_M = 0x06,
    LSM303DLHC_ADD_OUT_Z_H_M = 0x07,
    LSM303DLHC_ADD_OUT_Z_L_M = 0x08,
    LSM303DLHC_ADD_SR_REG_M = 0x09,
    LSM303DLHC_ADD_IRA_REG_M = 0x0A,
    LSM303DLHC_ADD_IRB_REG_M = 0x0B,
    LSM303DLHC_ADD_IRC_REG_M = 0x0C,
} Lsm303dlhcRegAdd;

#define LSM303DLHC_BUSSY_CNT_MAX    10000

Lsm303dlhcStatus lsm303dlhcMInit(Lsm303dlhcH *handler, I2cTxCb txCb, I2cRxCb rxCb)
{
    uint8_t verifyData[LSM303DLHC_VERIFY_DATA_SIZE];
    bool cbResult;

    if (handler == NULL) {
        return LSM303DLHC_STATUS_HANDLER_NULL_ERROR;
    }
    if (txCb == NULL || rxCb == NULL) {
        return LSM303DLHC_STATUS_CB_NULL_ERROR;
    }

    handler->rxCb = rxCb;
    handler->txCb = txCb;

    /*
     * Read IRA_REG_M, IRB_REG_M, IRC_REG_M registers to verify that sensor on the line
     */
    if (handler->rxCb(LSM303DLHC_M_ADDRESS, LSM303DLHC_ADD_IRA_REG_M,
                      LSM303DLHC_VERIFY_DATA_SIZE, verifyData) == false) {
        return LSM303DLHC_STATUS_CB_ERROR;
    }

    if (verifyData[0] != LSM303DLHC_VAL_IRA_REG_M
        || verifyData[1] != LSM303DLHC_VAL_IRB_REG_M
        || verifyData[2] != LSM303DLHC_VAL_IRC_REG_M)
    {
        return LSM303DLHC_OUT_OF_LINE_ERROR;
    }

    return LSM303DLHC_STATUS_OK;
}

#define LSM303DLHC_CRA_REG_M_DO_POS     2
#define LSM303DLHC_CRA_REG_M_DO_MASK    7
#define LSM303DLHC_CRA_REG_M_DO_0_75    0
#define LSM303DLHC_CRA_REG_M_DO_1_5     1
#define LSM303DLHC_CRA_REG_M_DO_3       2
#define LSM303DLHC_CRA_REG_M_DO_7_5     3
#define LSM303DLHC_CRA_REG_M_DO_15      4
#define LSM303DLHC_CRA_REG_M_DO_30      5
#define LSM303DLHC_CRA_REG_M_DO_75      6
#define LSM303DLHC_CRA_REG_M_DO_220     7

#define LSM303DLHC_CRB_REG_M_GN_POS     5
#define LSM303DLHC_CRB_REG_M_GN_MASK    7
#define LSM303DLHC_CRB_REG_M_GN_0       1
#define LSM303DLHC_CRB_REG_M_GN_1       2
#define LSM303DLHC_CRB_REG_M_GN_2       3
#define LSM303DLHC_CRB_REG_M_GN_3       4
#define LSM303DLHC_CRB_REG_M_GN_4       5
#define LSM303DLHC_CRB_REG_M_GN_5       6
#define LSM303DLHC_CRB_REG_M_GN_6       7

Lsm303dlhcStatus lsm303dlhcMSetRate(Lsm303dlhcH *handler, Lsm303dlhcMRate rate)
{
    uint8_t rate = 0;
    uint32_t cnt = 0;

    if (handler == NULL) {
        return LSM303DLHC_STATUS_HANDLER_NULL_ERROR;
    }

    if (rate > LSM303DLHC_M_RATE_220) {
        return LSM303DLHC_RATE_ERROR;
    }

    if (handler->bussy == true) {
        return LSM303DLHC_BUSSY_ERROR;
    }

    rate = (rate & LSM303DLHC_CRA_REG_M_DO_MASK) << LSM303DLHC_CRA_REG_M_DO_POS;

    /*
     * Set Rate;
     */
    handler->bussy = false;
    if (handler->txCb(LSM303DLHC_M_ADDRESS, LSM303DLHC_ADD_CRA_REG_M,
                      1, &rate) == false) {
        return LSM303DLHC_STATUS_CB_ERROR;
    }

    while (handler->bussy && cnt++ < LSM303DLHC_BUSSY_CNT_MAX) {}
    if (cnt == LSM303DLHC_BUSSY_CNT_MAX) {
        retur
    }
    /*
     * Read for verify Rate;
     */
    if (handler->rxCb(LSM303DLHC_M_ADDRESS, LSM303DLHC_ADD_CRA_REG_M,
                      1, &rate) == false) {
        return LSM303DLHC_STATUS_CB_ERROR;
    }

    return (((rate >>  LSM303DLHC_CRA_REG_M_DO_POS)  & LSM303DLHC_CRA_REG_M_DO_MASK)
            == rate & LSM303DLHC_CRA_REG_M_DO_MASK)
            ? LSM303DLHC_STATUS_OK
            : LSM303DLHC_VERIFY_ERROR;
}

Lsm303dlhcStatus lsm303dlhcMSetGain(Lsm303dlhcH *handler, Lsm303dlhcMGain gain)
{
    uint8_t gain = 0;

    if (handler == NULL) {
        return LSM303DLHC_STATUS_HANDLER_NULL_ERROR;
    }

    if (gain > LSM303DLHC_M_GAIN_6) {
        return LSM303DLHC_GAIN_ERROR;
    }

    gain = (gain & LSM303DLHC_CRB_REG_M_GN_MASK) << LSM303DLHC_CRB_REG_M_GN_POS;

    /*
     * Set Rate;
     */
    if (handler->txCb(LSM303DLHC_M_ADDRESS, LSM303DLHC_ADD_CRB_REG_M,
                      1, &gain) == false) {
        return LSM303DLHC_STATUS_CB_ERROR;
    }

    /*
     * Read for verify Rate;
     */
    if (handler->rxCb(LSM303DLHC_M_ADDRESS, LSM303DLHC_ADD_CRB_REG_M,
                      1, &gain) == false) {
        return LSM303DLHC_STATUS_CB_ERROR;
    }

    return (((gain >>  LSM303DLHC_CRB_REG_M_GN_POS)  & LSM303DLHC_CRB_REG_M_GN_MASK)
            == rate & LSM303DLHC_CRB_REG_M_GN_MASK)
            ? LSM303DLHC_STATUS_OK
            : LSM303DLHC_VERIFY_ERROR;
}