#include "I2c.h"
#include <stddef.h>
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_rcc.h"
#include "BSP.h"
#include "services.h"
#include "DebugServices.h"

#define I2C_CONCAT_2(X, Y)     X##_##Y
#define I2C_CONCAT(X, Y)       I2C_CONCAT_2(X, Y)

#define I2C_RX_ADDR(X)         ((X << 1) + 1)
#define I2C_TX_ADDR(X)         (X << 1)

#define I2C_WAITE_BUSSY_CNT    10000

/*
 * PE  - packet error
 * PEC - Packet error check
 */

typedef enum {
    I2C_LSM303DLHC_STATE_TX_SB,
    I2C_LSM303DLHC_STATE_TX_ADDRESS,
    I2C_LSM303DLHC_STATE_TX_COMLETE, // Set from the DMA
} I2CLsm303dlhcState;

static struct {
    I2cLsm303dlhcCb cb;
    uint8_t address;
    I2CLsm303dlhcState state;
    uint8_t *buff; // Tx Rx data buffer
    uint32_t buffSize; // Tx Rx data size
} i2cLsm3030dlhcState;

static I2cResult i2cLsm303dlhcConfig(const Lsm303dlhcSettings *settings)
{
    i2cLsm3030dlhcState.cb = settings->cb;
    LL_DMA_InitTypeDef dmaInitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_RCC_ClocksTypeDef rcc_clocks;

    /*
     ********************************GPIO*******************************************
     */

    servicesEnablePerephr(LSM303DLHC_I2C_GPIO_PORT);
    GPIO_InitStruct.Pin = LSM303DLHC_I2C_GPIO_SCL_PIN | LSM303DLHC_I2C_GPIO_SDA_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(LSM303DLHC_I2C_GPIO_PORT, &GPIO_InitStruct);

    /*
     ********************************DMA*******************************************
     */
    servicesEnablePerephr(LSM303DLHC_I2C_DMA);

    /*
     * Configure DMA I2C Tx stream
     */
    dmaInitStruct.PeriphOrM2MSrcAddress = LL_I2C_DMA_GetRegAddr(LSM303DLHC_I2C);
    dmaInitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dmaInitStruct.Mode = LL_DMA_MODE_NORMAL;
    dmaInitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dmaInitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dmaInitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
    dmaInitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
    dmaInitStruct.Channel = LSM303DLHC_I2C_TX_DMA_CHANNEL;
    dmaInitStruct.Priority = LL_DMA_PRIORITY_MEDIUM;
    dmaInitStruct.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
    dmaInitStruct.FIFOThreshold = 0;
    dmaInitStruct.MemBurst = LL_DMA_MBURST_SINGLE;
    dmaInitStruct.PeriphBurst = LL_DMA_PBURST_SINGLE;
    dmaInitStruct.MemoryOrM2MDstAddress = 0;
    dmaInitStruct.NbData = 0;

    LL_DMA_Init(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM, &dmaInitStruct);

    /*
     * Configure DMA I2C Rx stream
     */
    dmaInitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    dmaInitStruct.Channel = LSM303DLHC_I2C_RX_DMA_CHANNEL;

    LL_DMA_Init(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_RX_DMA_STREAM, &dmaInitStruct);

    LL_DMA_EnableIT_TC(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM);
    LL_DMA_EnableIT_TE(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM); // Transfer error interrupt
    NVIC_SetPriority(LSM303DLHC_I2C_TX_DMA_STREAM_IRQ, 6);

    /*
     ********************************I2C*******************************************
     */

    servicesEnablePerephr(LSM303DLHC_I2C);

    LL_I2C_Disable(LSM303DLHC_I2C);
    LL_RCC_GetSystemClocksFreq(&rcc_clocks);
    LL_I2C_ConfigSpeed(LSM303DLHC_I2C, rcc_clocks.PCLK1_Frequency, 400000, LL_I2C_DUTYCYCLE_2);
    LL_I2C_Enable(LSM303DLHC_I2C);
    LL_I2C_EnableIT_EVT(LSM303DLHC_I2C);
    LL_I2C_EnableIT_ERR(LSM303DLHC_I2C);

    NVIC_SetPriority(I2C1_EV_IRQn, 6);
    NVIC_SetPriority(I2C1_ER_IRQn, 6);

    return I2C_OK;
}

static I2cResult i2cLsm303dlhcTx(uint8_t devAddr, uint8_t *buff, uint32_t size)
{
    uint32_t cntBussy = I2C_WAITE_BUSSY_CNT;

    if (buff == NULL) {
        return I2C_BUFF_NULL_ERROR;
    }

    if (size == 0) {
        return I2C_DATA_SIZE_0_ERROR;
    }

    /*
     * Waite to complete last Transaction
     */
    while (LL_I2C_IsActiveFlag_BUSY(LSM303DLHC_I2C) && cntBussy--) {
    }

    if (cntBussy == 0) {
        if (i2cLsm3030dlhcState.cb.transactionComplete != NULL) {
            i2cLsm3030dlhcState.cb.transactionComplete(false);
        }
        return I2C_BUSSY;
    }

    i2cLsm3030dlhcState.address = I2C_TX_ADDR(devAddr);
    i2cLsm3030dlhcState.buff = buff;
    i2cLsm3030dlhcState.buffSize = size;
    i2cLsm3030dlhcState.state = I2C_LSM303DLHC_STATE_TX_SB;

    LL_DMA_SetDataLength(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM, size);
    LL_DMA_SetMemoryAddress(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM, (uint32_t)buff);

    LL_I2C_ClearFlag_STOP(LSM303DLHC_I2C);
    LL_I2C_ClearFlag_ADDR(LSM303DLHC_I2C);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    LL_I2C_GenerateStartCondition(LSM303DLHC_I2C);

    return I2C_OK;
}

static inline void i2cLsm303dlhcIrqH(void)
{
    bool isError = false;
    if (LSM303DLHC_I2C->SR1 == 0 && LSM303DLHC_I2C->SR2 == 0) {
        return;
    }

    switch (i2cLsm3030dlhcState.state) {
    case I2C_LSM303DLHC_STATE_TX_SB:
        if (LL_I2C_IsActiveFlag_SB(LSM303DLHC_I2C) == false) {
            isError = true;
        } else {
            i2cLsm3030dlhcState.state = I2C_LSM303DLHC_STATE_TX_ADDRESS;
            LL_I2C_TransmitData8(LSM303DLHC_I2C, i2cLsm3030dlhcState.address);
        }
        break;

    case I2C_LSM303DLHC_STATE_TX_ADDRESS: {
        LL_I2C_EnableDMAReq_TX(LSM303DLHC_I2C);// The DMAEN bit must be set before ADDR event

        uint32_t addrStatus = LL_I2C_IsActiveFlag_ADDR(LSM303DLHC_I2C);
        uint32_t mslStatus = LL_I2C_IsActiveFlag_MSL(LSM303DLHC_I2C);

        if (mslStatus) {
            if (addrStatus == 0) {
                isError = true;
            } else {
                /*
                * Start DMA
                */
                i2cLsm3030dlhcState.state = I2C_LSM303DLHC_STATE_TX_COMLETE;
                NVIC_EnableIRQ(LSM303DLHC_I2C_TX_DMA_STREAM_IRQ);
                LL_DMA_EnableStream(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM);
            }
        }
        break;
    }

    case I2C_LSM303DLHC_STATE_TX_COMLETE:
        if (LL_I2C_IsActiveFlag_BTF(LSM303DLHC_I2C) == false) {
            isError = true;
        } else {
            /*
             * Transmite complete
             */
            LL_I2C_GenerateStopCondition(LSM303DLHC_I2C);
            if (i2cLsm3030dlhcState.cb.transactionComplete != NULL) {
                i2cLsm3030dlhcState.cb.transactionComplete(true);
            }
            NVIC_DisableIRQ(I2C1_EV_IRQn);
            NVIC_DisableIRQ(I2C1_ER_IRQn);
        }
        break;
    }

    /*
     * Error occure
     */
    if (isError) {
        i2cLsm3030dlhcState.cb.transactionComplete(false);
        NVIC_DisableIRQ(I2C1_EV_IRQn);
        NVIC_DisableIRQ(I2C1_ER_IRQn);
    }
}

void DMA1_Stream1_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC1(LSM303DLHC_I2C_DMA)) {
        LL_DMA_ClearFlag_TC1(LSM303DLHC_I2C_DMA);
    } else if(LL_DMA_IsActiveFlag_TE1(LSM303DLHC_I2C_DMA)) {
        /* Transfer error */
        LL_DMA_ClearFlag_TE1(LSM303DLHC_I2C_DMA);
    }
    NVIC_DisableIRQ(LSM303DLHC_I2C_TX_DMA_STREAM_IRQ);
    LL_I2C_DisableDMAReq_TX(LSM303DLHC_I2C);// The DMAEN bit must be set before ADDR event
}

void I2C1_EV_IRQHandler(void)
{
    i2cLsm303dlhcIrqH();
}

/*
 * I2C line error processing
 */
void I2C1_ER_IRQHandler(void)
{
    if (LL_I2C_IsActiveFlag_AF(LSM303DLHC_I2C)) {
        LL_I2C_ClearFlag_AF(LSM303DLHC_I2C);
    }
    if (LL_I2C_IsActiveFlag_BERR(LSM303DLHC_I2C)) {
        LL_I2C_ClearFlag_BERR(LSM303DLHC_I2C);
    }
    if (i2cLsm3030dlhcState.cb.transactionComplete) {
        i2cLsm3030dlhcState.cb.transactionComplete(false);
    }
}

I2cResult i2cInit(I2cTarget i2cTarget, I2cSettings settings)
{
    I2cResult result;

    switch (i2cTarget) {
    case I2C_TARGET_LSM303DLHC:
        result = i2cLsm303dlhcConfig(&settings.lsm303dlhc);
        break;

    default:
        result = I2C_TARGET_ERROR;
    }

    return result;
}

I2cResult i2cTx(I2cTarget i2cTarget, uint8_t devAddr, uint8_t *buff, uint32_t size)
{
    I2cResult result;

    switch (i2cTarget) {
    case I2C_TARGET_LSM303DLHC:
        result = i2cLsm303dlhcTx( devAddr, buff, size);
        break;

    default:
        result = I2C_TARGET_ERROR;
        break;
    }

    return result;
}