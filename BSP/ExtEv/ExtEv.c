#include <stdint.h>
#include <stddef.h>

#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_gpio.h"
#include "BSP.h"
#include "services.h"
#include "ExtEv.h"

static ExtEvLsm303dlhcSettings lsm303dlhcSettings;

static ExtEvResult extEvLsm303dlhcConfig(ExtEvLsm303dlhcSettings *settings)
{
    LL_EXTI_InitTypeDef extIntnitStruct;
    LL_GPIO_InitTypeDef gpioInitSruct;

    lsm303dlhcSettings = *settings;

    /*
     * Init Gpio
     */
    servicesEnablePerephr(LSM303DLHC_DRDY_GPIO_PORT);

    gpioInitSruct.Mode = LL_GPIO_MODE_INPUT;
    gpioInitSruct.Alternate = LL_GPIO_AF_0; // not used
    gpioInitSruct.Pin = LSM303DLHC_DRDY_GPIO_PIN;
    gpioInitSruct.Pull= LL_GPIO_PULL_NO;
    gpioInitSruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;

    LL_GPIO_Init(LSM303DLHC_DRDY_GPIO_PORT, &gpioInitSruct);

    /*
     * To use external interrupt lines, the port must be configured in input mode
     */

    /*
     * Init external interrupt
     */
    extIntnitStruct.Line_0_31 = LSM303DLHC_DRDY_EXT_LINE;
    extIntnitStruct.LineCommand = ENABLE;
    extIntnitStruct.Mode = LL_EXTI_MODE_IT;
    extIntnitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;

    LL_EXTI_Init(&extIntnitStruct);
    LL_SYSCFG_SetEXTISource(LSM303DLHC_DRDY_SYS_PORT, LSM303DLHC_DRDY_SYS_LINE);

    if (lsm303dlhcSettings.extEvCb != NULL) {
        NVIC_EnableIRQ(LSM303DLHC_DRDY_IRQ);
    }

    return EXT_EV_OK;
}

void LSM303DLHC_DRDY_IRQHandler(void)
{
    if ((lsm303dlhcSettings.extEvCb != NULL)
        && (LL_EXTI_IsActiveFlag_0_31(LSM303DLHC_DRDY_EXT_LINE) == true)) {
        lsm303dlhcSettings.extEvCb();
    }
    LL_EXTI_ClearFlag_0_31(LSM303DLHC_DRDY_EXT_LINE);
}

ExtEvResult extEvInit(ExtEvTarget extEvTarget, ExtEvSettings settings)
{
    ExtEvResult result;

    switch (extEvTarget) {
    case EXT_EV_TARGET_LSM303DLHC:
        result = extEvLsm303dlhcConfig(&settings.lsm303dlhc);
        break;

    default:
        result = EXT_EV_TARGET_ERROR;
    }

    return result;
}