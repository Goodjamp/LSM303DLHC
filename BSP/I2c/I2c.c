#include "I2c.h"
#include <stddef.h>
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_rcc.h"
#include "BSP.h"
#include "services.h"

#define SSD1306_I2C_ADDR 0x78

static I2cLsm303dlhcCb lsm303dhlcCb;

void DMA1_Stream1_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC1(LSM303DLHC_I2C_DMA)) {
        LL_DMA_ClearFlag_TC1(LSM303DLHC_I2C_DMA);
    } else if(LL_DMA_IsActiveFlag_TE1(LSM303DLHC_I2C_DMA)) {
        /* Transfer error */
        LL_DMA_ClearFlag_TE1(LSM303DLHC_I2C_DMA);
    }
}

void I2C1_EV_IRQHandler(void)
{
    if (LL_I2C_IsActiveFlag_SB(LSM303DLHC_I2C)) {
        LL_I2C_TransmitData8(LSM303DLHC_I2C, SSD1306_I2C_ADDR);
    } else if(LL_I2C_IsActiveFlag_ADDR(LSM303DLHC_I2C)) {
        LL_I2C_EnableDMAReq_TX(LSM303DLHC_I2C);
        LL_I2C_ClearFlag_ADDR(LSM303DLHC_I2C);
    }

    if (LL_I2C_IsActiveFlag_BTF(LSM303DLHC_I2C)) {
        LL_I2C_GenerateStopCondition(LSM303DLHC_I2C);

        if (lsm303dhlcCb.transactionComplete) {
            lsm303dhlcCb.transactionComplete(true);
        }
    }
}

void I2C1_ER_IRQHandler(void)
{
    if (LL_I2C_IsActiveFlag_AF(LSM303DLHC_I2C)) {
        LL_I2C_ClearFlag_AF(LSM303DLHC_I2C);
    }
    if (LL_I2C_IsActiveFlag_BERR(LSM303DLHC_I2C)) {
        LL_I2C_ClearFlag_BERR(LSM303DLHC_I2C);
    }
    if (lsm303dhlcCb.transactionComplete) {
        lsm303dhlcCb.transactionComplete(false);
    }
}

static I2cResult i2cLsm303dlhcConfig(const Lsm303dlhcSettings *settings)
{
    lsm303dhlcCb = settings->cb;

    // DMA
    servicesEnablePerephr(LSM303DLHC_I2C_DMA);
    NVIC_SetPriority(DMA1_Stream1_IRQn, 6);
    NVIC_EnableIRQ(DMA1_Stream1_IRQn);

    LL_DMA_SetChannelSelection(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM, LSM303DLHC_I2C_TX_DMA_CHANNEL);

    LL_DMA_ConfigTransfer(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM, LL_DMA_DIRECTION_MEMORY_TO_PERIPH | \
                                                LL_DMA_PRIORITY_HIGH              | \
                                                LL_DMA_MODE_NORMAL                | \
                                                LL_DMA_PERIPH_NOINCREMENT           | \
                                                LL_DMA_MEMORY_INCREMENT           | \
                                                LL_DMA_PDATAALIGN_BYTE            | \
                                                LL_DMA_MDATAALIGN_BYTE);

    LL_DMA_EnableIT_TC(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM);
    LL_DMA_EnableIT_TE(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM);
    // GPIO

    servicesEnablePerephr(LSM303DLHC_I2C_GPIO_PORT);
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LSM303DLHC_I2C_GPIO_SCL_PIN | LSM303DLHC_I2C_GPIO_SDA_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(LSM303DLHC_I2C_GPIO_PORT, &GPIO_InitStruct);
    // I2C
    servicesEnablePerephr(LSM303DLHC_I2C);
    NVIC_SetPriority(I2C1_EV_IRQn, 6);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_SetPriority(I2C1_ER_IRQn, 6);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    LL_I2C_Disable(LSM303DLHC_I2C);
    LL_RCC_ClocksTypeDef rcc_clocks;
    LL_RCC_GetSystemClocksFreq(&rcc_clocks);
    LL_I2C_ConfigSpeed(LSM303DLHC_I2C, rcc_clocks.PCLK1_Frequency, 400000, LL_I2C_DUTYCYCLE_2);
    LL_I2C_Enable(LSM303DLHC_I2C);
    LL_I2C_EnableIT_EVT(LSM303DLHC_I2C);
    LL_I2C_EnableIT_ERR(LSM303DLHC_I2C);
    return I2C_OK;
}

static I2cResult i2cSsd1306Tx(uint8_t *buff, uint32_t size)
{
    if (buff == NULL) {
        return I2C_BUFF_NULL_ERROR;
    }

    if (size == 0) {
        return I2C_DATA_SIZE_0_ERROR;
    }

    LL_DMA_SetDataLength(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM, size);
    uint32_t dir = LL_DMA_GetDataTransferDirection(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM);
    LL_DMA_ConfigAddresses(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM, (uint32_t)(buff), LL_I2C_DMA_GetRegAddr(LSM303DLHC_I2C), dir);
    LL_DMA_EnableStream(LSM303DLHC_I2C_DMA, LSM303DLHC_I2C_TX_DMA_STREAM);
    LL_I2C_AcknowledgeNextData(LSM303DLHC_I2C, LL_I2C_ACK);
    LL_I2C_GenerateStartCondition(LSM303DLHC_I2C);

    return I2C_OK;
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

I2cResult i2cTx(I2cTarget i2cTarget, uint8_t devAddr, uint8_t regAddr, uint8_t *buff, uint32_t size)
{
    I2cResult result;

    switch (i2cTarget) {
    case I2C_TARGET_LSM303DLHC:
        result = i2cSsd1306Tx(uint8_t devAddr, uint8_t regAddr, uint8_t *buff, uint32_t size);
        break;

    default:
        result = I2C_TARGET_ERROR;
        break;
    }

    return result;
}