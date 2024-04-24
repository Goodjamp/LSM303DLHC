#include <stdint.h>
#include <stdlib.h>

#include "Lsm303dlhc.h"

#define BYTE_TO_UINT16(H, L)    (((H << 8) & 0xFF00) | (L & 0xFF))

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

#define LSM303DLHC_M_ADDRESS                 0x1E
#define LSM303DLHC_VAL_IRA_REG_M             0x48
#define LSM303DLHC_VAL_IRB_REG_M             0x34
#define LSM303DLHC_VAL_IRC_REG_M             0x33
#define LSM303DLHC_VERIFY_DATA_SIZE          3

/*
 * Register CRA_REG_M, field DO (conversion rate) description
 */
#define LSM303DLHC_CRA_REG_M_DO_POS          2
#define LSM303DLHC_CRA_REG_M_DO_MASK         7
#define LSM303DLHC_CRA_REG_M_DO_0_75         0
#define LSM303DLHC_CRA_REG_M_DO_1_5          1
#define LSM303DLHC_CRA_REG_M_DO_3            2
#define LSM303DLHC_CRA_REG_M_DO_7_5          3
#define LSM303DLHC_CRA_REG_M_DO_15           4
#define LSM303DLHC_CRA_REG_M_DO_30           5
#define LSM303DLHC_CRA_REG_M_DO_75           6
#define LSM303DLHC_CRA_REG_M_DO_220          7

/*
 * Register CRB_REG_M, field GN (gaine) description
 */
#define LSM303DLHC_CRB_REG_M_GN_POS          5
#define LSM303DLHC_CRB_REG_M_GN_MASK         7
#define LSM303DLHC_CRB_REG_M_GN_0            1
#define LSM303DLHC_CRB_REG_M_GN_1            2
#define LSM303DLHC_CRB_REG_M_GN_2            3
#define LSM303DLHC_CRB_REG_M_GN_3            4
#define LSM303DLHC_CRB_REG_M_GN_4            5
#define LSM303DLHC_CRB_REG_M_GN_5            6
#define LSM303DLHC_CRB_REG_M_GN_6            7

/*
 * Register MR_REG_M, field MD (operation mode) description
 */
#define LSM303DLHC_MR_REG_M_MD_POS           0
#define LSM303DLHC_MR_REG_M_MD_MASK          3
#define LSM303DLHC_MR_REG_M_MD_CONT_CONV     0
#define LSM303DLHC_MR_REG_M_MD_SINGL_CONV    1
#define LSM303DLHC_MR_REG_M_MD_SLEEP         3

/*
 * The timeoute to cpmplete I2C transaction
 */
#define LSM303DLHC_BUSSY_CNT_MAX             10000

typedef enum {
    LSM303DLHC_STATE_NON,
    LSM303DLHC_STATE_MES_BLOCKING,
    LSM303DLHC_STATE_MES_NO_BLOCKING,
} Lsm303dlhcState;

typedef struct Lsm303dlhcS {
    I2cTxCb txCb;
    I2cRxCb rxCb;
    Lsm303dlhcMMesCompleteCb mMesCompleteCb;
    volatile Lsm303dlhcState state;
    volatile bool bussy;
    volatile bool dataReady;
    uint8_t regAddress;
    uint8_t mData[6];
} Lsm303dlhcH;

#if !defined(LSM303DLHC_ITEMS_NUMBER) | LSM303DLHC_ITEMS_NUMBER < 1
#error The LSM303DLHC_ITEMS_NUMBER must be defined and >= 1
#endif

static struct {
    Lsm303dlhcH list[LSM303DLHC_ITEMS_NUMBER];
    uint32_t index;
} handlerList = {
    .index = 0,
};

Lsm303dlhcHandler lsm303dlhcTakeHandler(void)
{
    return (handlerList.index == LSM303DLHC_ITEMS_NUMBER)
           ? NULL
           : &handlerList.list[handlerList.index++];
}

static inline bool lsm303dlhcWait(Lsm303dlhcHandler handler)
{
    volatile uint32_t cnt = 0;

    while (handler->bussy && cnt++ < LSM303DLHC_BUSSY_CNT_MAX) {}
    return cnt == LSM303DLHC_BUSSY_CNT_MAX;
}

static inline Lsm303dlhcStatus lsm303dlhcGetReg(Lsm303dlhcHandler handler, uint8_t address,
                                                uint8_t *value)
{
    handler->regAddress = address;

    handler->bussy = true;
    if (handler->rxCb(LSM303DLHC_M_ADDRESS, &handler->regAddress,
                      value, 1) == false) {
        return LSM303DLHC_STATUS_CB_ERROR;
    }

    /*
     * waite to complete transaction
     */
    if (lsm303dlhcWait(handler) == true) {
        return LSM303DLHC_STATUS_TIMEOUTE_ERROR;
    }

    return LSM303DLHC_STATUS_OK;
}

static inline Lsm303dlhcStatus lsm303dlhcSetReg(Lsm303dlhcHandler handler, uint8_t address,
                                                uint8_t value)
{
    uint8_t txData[] = {address, value};

    handler->bussy = true;
    if (handler->txCb(LSM303DLHC_M_ADDRESS, txData, sizeof(txData)) == false) {
        return LSM303DLHC_STATUS_CB_ERROR;
    }

    /*
     * Waite to complete transaction
     */
    if (lsm303dlhcWait(handler) == true) {
        return LSM303DLHC_STATUS_TIMEOUTE_ERROR;
    }

    return LSM303DLHC_STATUS_OK;
}

Lsm303dlhcHandler lsm303dlhcMInit(I2cTxCb txCb, I2cRxCb rxCb)
{
    Lsm303dlhcHandler handler;

    if (txCb == NULL || rxCb == NULL) {
        return NULL;
    }

    handler = lsm303dlhcTakeHandler();

    if (handler == NULL) {
        return NULL;
    }

    handler->rxCb = rxCb;
    handler->txCb = txCb;
    handler->bussy = false;

    return handler;
}

Lsm303dlhcStatus lsm303dlhcIsConnected(Lsm303dlhcHandler handler, bool *isConnect)
{
    uint8_t verifyData[LSM303DLHC_VERIFY_DATA_SIZE];

    if (handler == NULL) {
        return LSM303DLHC_STATUS_HANDLER_NULL_ERROR;
    }

    *isConnect = true;

    /*
     * Read IRA_REG_M, IRB_REG_M, IRC_REG_M registers to verify that sensor on the line
     */
    handler->regAddress = LSM303DLHC_ADD_IRA_REG_M;

    if (handler->rxCb(LSM303DLHC_M_ADDRESS, &handler->regAddress,
                      verifyData, LSM303DLHC_VERIFY_DATA_SIZE) == false) {
        return LSM303DLHC_STATUS_CB_ERROR;
    }

    /*
     * waite to complete transaction
     */
    if (lsm303dlhcWait(handler) == true) {
        return LSM303DLHC_STATUS_TIMEOUTE_ERROR;
    }

    if (verifyData[0] != LSM303DLHC_VAL_IRA_REG_M
        || verifyData[1] != LSM303DLHC_VAL_IRB_REG_M
        || verifyData[2] != LSM303DLHC_VAL_IRC_REG_M)
    {
        *isConnect = false;
    }

    return LSM303DLHC_STATUS_OK;
}

Lsm303dlhcStatus lsm303dlhcMesMStop(Lsm303dlhcHandler handler)
{
    uint8_t mode = (LSM303DLHC_MR_REG_M_MD_SLEEP << LSM303DLHC_MR_REG_M_MD_POS) & 0xFF;
    Lsm303dlhcStatus result;

    if (handler == NULL) {
        return LSM303DLHC_STATUS_HANDLER_NULL_ERROR;
    }

    if (handler->bussy == true) {
        return LSM303DLHC_STATUS_BUSSY_ERROR;
    }

    /*
     * Disable sensor
     */
    result = lsm303dlhcSetReg(handler, LSM303DLHC_ADD_MR_REG_M, mode);
    if (result != LSM303DLHC_STATUS_OK)
    {
        handler->state = LSM303DLHC_STATE_NON;
        return result;
    }

    return LSM303DLHC_STATUS_OK;
}

static inline void lsm303dlhcGetAngle(Lsm303dlhcHandler handler)
{
    Lsm303dlhcMagnetic magnetic = {BYTE_TO_UINT16(handler->mData[0], handler->mData[1]),
                                   BYTE_TO_UINT16(handler->mData[4], handler->mData[5]),
                                   BYTE_TO_UINT16(handler->mData[2], handler->mData[3])};
    uint16_t angle = 0;

    if (handler->mMesCompleteCb != NULL) {
        handler->mMesCompleteCb(magnetic, angle);
    }
}

void lsm303dlhcI2cComplete(Lsm303dlhcHandler handler)
{
    handler->bussy = false;

    switch (handler->state) {
    case LSM303DLHC_STATE_NON:
    case LSM303DLHC_STATE_MES_BLOCKING:
        break;

    case LSM303DLHC_STATE_MES_NO_BLOCKING:
        lsm303dlhcGetAngle(handler);
        break;
    }
}

void lsm303dlhcDrdy(Lsm303dlhcHandler handler)
{
    switch (handler->state) {
    case LSM303DLHC_STATE_NON:
        break;

    case LSM303DLHC_STATE_MES_BLOCKING:
    case LSM303DLHC_STATE_MES_NO_BLOCKING:
        handler->regAddress = LSM303DLHC_ADD_OUT_X_H_M;
        handler->rxCb(LSM303DLHC_M_ADDRESS, &handler->regAddress,
                      handler->mData, sizeof(handler->mData));
        break;
    }
}

Lsm303dlhcStatus lsm303dlhcMSetRate(Lsm303dlhcHandler handler, Lsm303dlhcMRate rate)
{
    uint8_t rateTx = 0;
    uint8_t rateRx = 0;
    Lsm303dlhcStatus result;

    if (handler == NULL) {
        return LSM303DLHC_STATUS_HANDLER_NULL_ERROR;
    }

    if (handler->bussy == true) {
        return LSM303DLHC_STATUS_BUSSY_ERROR;
    }

    switch (rate) {
    case LSM303DLHC_M_RATE_0_75:
        rateTx = (LSM303DLHC_CRA_REG_M_DO_0_75 << LSM303DLHC_CRA_REG_M_DO_POS) & 0xFF;
        break;

    case LSM303DLHC_M_RATE_1_5:
        rateTx = (LSM303DLHC_CRA_REG_M_DO_1_5 << LSM303DLHC_CRA_REG_M_DO_POS) & 0xFF;
        break;

    case LSM303DLHC_M_RATE_3:
        rateTx = (LSM303DLHC_CRA_REG_M_DO_3 << LSM303DLHC_CRA_REG_M_DO_POS) & 0xFF;
        break;

    case LSM303DLHC_M_RATE_7_5:
        rateTx = (LSM303DLHC_CRA_REG_M_DO_7_5 << LSM303DLHC_CRA_REG_M_DO_POS) & 0xFF;
        break;

    case LSM303DLHC_M_RATE_15:
        rateTx = (LSM303DLHC_CRA_REG_M_DO_15 << LSM303DLHC_CRA_REG_M_DO_POS) & 0xFF;
        break;

    case LSM303DLHC_M_RATE_30:
        rateTx = (LSM303DLHC_CRA_REG_M_DO_30 << LSM303DLHC_CRA_REG_M_DO_POS) & 0xFF;
        break;

    case LSM303DLHC_M_RATE_75:
        rateTx = (LSM303DLHC_CRA_REG_M_DO_75 << LSM303DLHC_CRA_REG_M_DO_POS) & 0xFF;
        break;

    case LSM303DLHC_M_RATE_220:
        rateTx = (LSM303DLHC_CRA_REG_M_DO_220 << LSM303DLHC_CRA_REG_M_DO_POS) & 0xFF;
        break;

    default:
        return LSM303DLHC_STATUS_RATE_ERROR;
    }

    /*
     * Disable sensor
     */
    result = lsm303dlhcMesMStop(handler);
    if (result != LSM303DLHC_STATUS_OK)
    {
        return result;
    }

    /*
     * Set Rate;
     */
    result = lsm303dlhcSetReg(handler, LSM303DLHC_ADD_CRA_REG_M, rateTx);
    if (result != LSM303DLHC_STATUS_OK)
    {
        return result;
    }

    /*
     * Read for verify Rate;
     */
    result = lsm303dlhcGetReg(handler, LSM303DLHC_ADD_CRA_REG_M, &rateRx);
    if (result != LSM303DLHC_STATUS_OK) {
        return result;
    }

    return ((rateTx  & (LSM303DLHC_CRA_REG_M_DO_MASK << LSM303DLHC_CRA_REG_M_DO_POS))
            == rateRx)
            ? LSM303DLHC_STATUS_OK
            : LSM303DLHC_STATUS_VERIFY_ERROR;
}

Lsm303dlhcStatus lsm303dlhcMSetGain(Lsm303dlhcHandler handler, Lsm303dlhcMGain gain)
{
    uint8_t gainTx = 0;
    uint8_t gainRx = 0;
    Lsm303dlhcStatus result;

    if (handler == NULL) {
        return LSM303DLHC_STATUS_HANDLER_NULL_ERROR;
    }

    if (handler->bussy == true) {
        return LSM303DLHC_STATUS_BUSSY_ERROR;
    }

    switch (gain) {
    case LSM303DLHC_M_GAIN_0:
        gainTx = (LSM303DLHC_CRB_REG_M_GN_0 << LSM303DLHC_CRB_REG_M_GN_POS) & 0xFF;
        break;

    case LSM303DLHC_M_GAIN_1:
        gainTx = (LSM303DLHC_CRB_REG_M_GN_1 << LSM303DLHC_CRB_REG_M_GN_POS) & 0xFF;
        break;

    case LSM303DLHC_M_GAIN_2:
        gainTx = (LSM303DLHC_CRB_REG_M_GN_2 << LSM303DLHC_CRB_REG_M_GN_POS) & 0xFF;
        break;

    case LSM303DLHC_M_GAIN_3:
        gainTx = (LSM303DLHC_CRB_REG_M_GN_3 << LSM303DLHC_CRB_REG_M_GN_POS) & 0xFF;
        break;

    case LSM303DLHC_M_GAIN_4:
        gainTx = (LSM303DLHC_CRB_REG_M_GN_4 << LSM303DLHC_CRB_REG_M_GN_POS) & 0xFF;
        break;

    case LSM303DLHC_M_GAIN_5:
        gainTx = (LSM303DLHC_CRB_REG_M_GN_5 << LSM303DLHC_CRB_REG_M_GN_POS) & 0xFF;
        break;

    case LSM303DLHC_M_GAIN_6:
        gainTx = (LSM303DLHC_CRB_REG_M_GN_6 << LSM303DLHC_CRB_REG_M_GN_POS) & 0xFF;
        break;

    default:
        return LSM303DLHC_STATUS_BUSSY_ERROR;
    }

    /*
     * Disable sensor
     */
    result = lsm303dlhcMesMStop(handler);
    if (result != LSM303DLHC_STATUS_OK)
    {
        return result;
    }

    /*
     * Set Gain
     */
    result = lsm303dlhcSetReg(handler, LSM303DLHC_ADD_CRB_REG_M, gainTx);
    if (result != LSM303DLHC_STATUS_OK)
    {
        return result;
    }

    /*
     * Read for verify Gain;
     */
    result = lsm303dlhcGetReg(handler, LSM303DLHC_ADD_CRB_REG_M, &gainRx);
    if (result != LSM303DLHC_STATUS_OK) {
        return result;
    }

    return ((gainRx & (LSM303DLHC_CRB_REG_M_GN_MASK << LSM303DLHC_CRB_REG_M_GN_POS)) == gainTx)
            ? LSM303DLHC_STATUS_OK
            : LSM303DLHC_STATUS_VERIFY_ERROR;
}

/*
 * 1 - disable sensor (to clear sensor state: if sensor in continuous conversion mode, we
 *     need disable measurements, )
 * 2 - clear
 */
static Lsm303dlhcStatus lsm303dlhcMesMNotBlocking(Lsm303dlhcHandler handler, Lsm303dlhcMMesCompleteCb mesCompleteCb, bool continuous)
{
    uint8_t mode = (((continuous == true)
                     ? LSM303DLHC_MR_REG_M_MD_CONT_CONV
                     : LSM303DLHC_MR_REG_M_MD_SINGL_CONV) << LSM303DLHC_MR_REG_M_MD_POS) & 0xFF;
    Lsm303dlhcStatus result;

    if (handler == NULL) {
        return LSM303DLHC_STATUS_HANDLER_NULL_ERROR;
    }

    if (handler->bussy == true) {
        return LSM303DLHC_STATUS_BUSSY_ERROR;
    }

    /*
     * Disable sensor
     */
    result = lsm303dlhcMesMStop(handler);
    if (result != LSM303DLHC_STATUS_OK)
    {
        return result;
    }

    handler->state = LSM303DLHC_STATE_MES_NO_BLOCKING;
    handler->mMesCompleteCb = mesCompleteCb;

    /*
     * Set Single mode (one measurement)
     */
    result = lsm303dlhcSetReg(handler, LSM303DLHC_ADD_MR_REG_M, mode);
    if (result != LSM303DLHC_STATUS_OK)
    {
        handler->state = LSM303DLHC_STATE_NON;
        return result;
    }

    return LSM303DLHC_STATUS_OK;
}

Lsm303dlhcStatus lsm303dlhcMesM(Lsm303dlhcHandler handler, Lsm303dlhcMMesCompleteCb mesCompleteCb)
{
    return lsm303dlhcMesMNotBlocking(handler, mesCompleteCb, false);
}

Lsm303dlhcStatus lsm303dlhcMesMStart(Lsm303dlhcHandler handler, Lsm303dlhcMMesCompleteCb mesCompleteCb)
{
    return lsm303dlhcMesMNotBlocking(handler, mesCompleteCb, true);
}