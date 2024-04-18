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

static I2cSsd1306Cb ssd1306Cb;

void DMA1_Stream1_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC1(SSD1306_I2C_DMA)) {
        LL_DMA_ClearFlag_TC1(SSD1306_I2C_DMA);
    } else if(LL_DMA_IsActiveFlag_TE1(SSD1306_I2C_DMA)) {
        /* Transfer error */
        LL_DMA_ClearFlag_TE1(SSD1306_I2C_DMA);
    }
}

void I2C1_EV_IRQHandler(void)
{
    if (LL_I2C_IsActiveFlag_SB(SSD1306_I2C)) {
        LL_I2C_TransmitData8(SSD1306_I2C, SSD1306_I2C_ADDR);
    } else if(LL_I2C_IsActiveFlag_ADDR(SSD1306_I2C)) {
        LL_I2C_EnableDMAReq_TX(SSD1306_I2C);
        LL_I2C_ClearFlag_ADDR(SSD1306_I2C);
    }

    if (LL_I2C_IsActiveFlag_BTF(SSD1306_I2C)) {
        LL_I2C_GenerateStopCondition(SSD1306_I2C);

        if (ssd1306Cb.txComplete) {
            ssd1306Cb.txComplete(true);
        }
    }
}

void I2C1_ER_IRQHandler(void)
{
    if (LL_I2C_IsActiveFlag_AF(SSD1306_I2C)) {
        LL_I2C_ClearFlag_AF(SSD1306_I2C);
    }
    if (LL_I2C_IsActiveFlag_BERR(SSD1306_I2C)) {
        LL_I2C_ClearFlag_BERR(SSD1306_I2C);
    }
    if (ssd1306Cb.txComplete) {
        ssd1306Cb.txComplete(false);
    }
}

static I2cResult i2cSsd1306Config(const Ssd1306Settings *settings, const I2cSsd1306Cb *cb)
{
    ssd1306Cb = *cb;
    // DMA
    servicesEnablePerephr(SSD1306_I2C_DMA);
    NVIC_SetPriority(DMA1_Stream1_IRQn, 6);
    NVIC_EnableIRQ(DMA1_Stream1_IRQn);

    LL_DMA_SetChannelSelection(SSD1306_I2C_DMA, SSD1306_I2C_DMA_STREAM, SSD1306_I2C_DMA_CHANNEL);

    LL_DMA_ConfigTransfer(SSD1306_I2C_DMA, SSD1306_I2C_DMA_STREAM, LL_DMA_DIRECTION_MEMORY_TO_PERIPH | \
                                                LL_DMA_PRIORITY_HIGH              | \
                                                LL_DMA_MODE_NORMAL                | \
                                                LL_DMA_PERIPH_NOINCREMENT           | \
                                                LL_DMA_MEMORY_INCREMENT           | \
                                                LL_DMA_PDATAALIGN_BYTE            | \
                                                LL_DMA_MDATAALIGN_BYTE);

    LL_DMA_EnableIT_TC(SSD1306_I2C_DMA, SSD1306_I2C_DMA_STREAM);
    LL_DMA_EnableIT_TE(SSD1306_I2C_DMA, SSD1306_I2C_DMA_STREAM);
    // GPIO

    servicesEnablePerephr(SSD1306_I2C_GPIO_PORT);
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SSD1306_I2C_GPIO_SCL_PIN | SSD1306_I2C_GPIO_SDA_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(SSD1306_I2C_GPIO_PORT, &GPIO_InitStruct);
    // I2C
    servicesEnablePerephr(SSD1306_I2C);
    NVIC_SetPriority(I2C1_EV_IRQn, 6);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_SetPriority(I2C1_ER_IRQn, 6);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    LL_I2C_Disable(SSD1306_I2C);
    LL_RCC_ClocksTypeDef rcc_clocks;
    LL_RCC_GetSystemClocksFreq(&rcc_clocks);
    LL_I2C_ConfigSpeed(SSD1306_I2C, rcc_clocks.PCLK1_Frequency, 400000, LL_I2C_DUTYCYCLE_2);
    LL_I2C_Enable(SSD1306_I2C);
    LL_I2C_EnableIT_EVT(SSD1306_I2C);
    LL_I2C_EnableIT_ERR(SSD1306_I2C);
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

    LL_DMA_SetDataLength(SSD1306_I2C_DMA, SSD1306_I2C_DMA_STREAM, size);
    uint32_t dir = LL_DMA_GetDataTransferDirection(SSD1306_I2C_DMA, SSD1306_I2C_DMA_STREAM);
    LL_DMA_ConfigAddresses(SSD1306_I2C_DMA, SSD1306_I2C_DMA_STREAM, (uint32_t)(buff), LL_I2C_DMA_GetRegAddr(SSD1306_I2C), dir);
    LL_DMA_EnableStream(SSD1306_I2C_DMA, SSD1306_I2C_DMA_STREAM);
    LL_I2C_AcknowledgeNextData(SSD1306_I2C, LL_I2C_ACK);
    LL_I2C_GenerateStartCondition(SSD1306_I2C);
    return I2C_OK;
}

I2cResult i2cInit(I2cTarget i2cTarget, I2cSettings settings, I2cCb cb)
{
    I2cResult result;

    switch (i2cTarget) {
    case I2C_TARGET_SSD1306:
        result = i2cSsd1306Config(&settings.ssd1306, &cb.ssd1306);
        break;

    default:
        result = I2C_TARGET_ERROR;
    }

    return result;
}

I2cResult i2cTx(I2cTarget i2cTarget, uint8_t *buff, uint32_t size)
{
    I2cResult result;

    switch (i2cTarget) {
    case I2C_TARGET_SSD1306:
        result = i2cSsd1306Tx(buff, size);
        break;

    default:
        result = I2C_TARGET_ERROR;
        break;
    }

    return result;
}