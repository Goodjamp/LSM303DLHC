#include <string.h>

#include "services.h"
#include "stm32f411xe.h"
#include "stm32f4xx_ll_bus.h"

bool servicesSetResetPerephr(void *pereph)
{
    uint32_t perepAddress;
    bool result = true;

    memcpy(&perepAddress, &pereph, sizeof(perepAddress));

	switch (perepAddress) {
    case I2C1_BASE:
        RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
        break;

    case I2C2_BASE:
        RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
        break;

    case I2C3_BASE:
        RCC->APB1RSTR |= RCC_APB1RSTR_I2C3RST;
        break;

	default:
	    result = false;
	}

	return result;
}

bool servicesClearResetPerephr(void *pereph)
{
    uint32_t perepAddress;
    bool result = true;

    memcpy(&perepAddress, &pereph, sizeof(perepAddress));

	switch (perepAddress) {
    case I2C1_BASE:
        RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST);
        break;

    case I2C2_BASE:
        RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C2RST);
        break;

    case I2C3_BASE:
        RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C3RST);
        break;

	default:
	    result = false;
	}

	return result;
}

bool servicesEnablePerephr(void *pereph)
{
    uint32_t perepAddress;
    bool result = true;
	
	memcpy(&perepAddress, &pereph, sizeof(perepAddress));

    switch (perepAddress) {
    case TIM2_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
        break;

    case TIM3_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
        break;

    case TIM4_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
        break;

    case TIM5_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);
        break;

    case WWDG_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);
        break;

    case SPI2_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
        break;

    case SPI3_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
        break;

    case USART2_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
        break;

    case I2C1_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
        break;

    case I2C2_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
        break;

    case I2C3_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);
        break;

    case PWR_BASE:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
        break;

    case TIM1_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
        break;

    case USART1_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
        break;

    case USART6_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART6);
        break;

    case ADC1_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
        break;

    case SDIO_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SDIO);
        break;

    case SPI1_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
        break;

    case SPI4_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI4);
        break;

    case SYSCFG_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
        break;

    case TIM9_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9);
        break;

    case TIM10_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM10);
        break;

    case TIM11_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM11);
        break;

    case SPI5_BASE:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI5);
        break;

    case GPIOA_BASE:
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
        break;

    case GPIOB_BASE:
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
        break;

    case GPIOC_BASE:
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
        break;

    case GPIOD_BASE:
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
        break;

    case CRC_BASE:
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);
        break;

    case DMA1_BASE:
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
        break;

    case DMA2_BASE:
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
        break;

    default:
        result = false;
        break;
    }

    return result;
}

bool servicesDisablePerephr(void *pereph)
{
    uint32_t perepAddress;
    memcpy(&perepAddress, &pereph, sizeof(perepAddress));
    bool result = true;

    switch (perepAddress) {
    case TIM2_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM2);
        break;

    case TIM3_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM3);
        break;

    case TIM4_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM4);
        break;

    case TIM5_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM5);
        break;

    case WWDG_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_WWDG);
        break;

    case SPI2_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_SPI2);
        break;

    case SPI3_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_SPI3);
        break;

    case USART2_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2);
        break;

    case I2C1_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
        break;

    case I2C2_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C2);
        break;

    case I2C3_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C3);
        break;

    case PWR_BASE:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
        break;

    case TIM1_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM1);
        break;

    case USART1_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1);
        break;

    case USART6_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART6);
        break;

    case ADC1_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);
        break;

    case SDIO_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SDIO);
        break;

    case SPI1_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI1);
        break;

    case SPI4_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI4);
        break;

    case SYSCFG_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
        break;

    case TIM9_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM9);
        break;

    case TIM10_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM10);
        break;

    case TIM11_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM11);
        break;

    case SPI5_BASE:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI5);
        break;

    case GPIOA_BASE:
        LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
        break;

    case GPIOB_BASE:
        LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
        break;

    case GPIOC_BASE:
        LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
        break;

    case GPIOD_BASE:
        LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
        break;

    case CRC_BASE:
        LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_CRC);
        break;

    case DMA1_BASE:
        LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA1);
        break;

    case DMA2_BASE:
        LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA2);
        break;

    default:
        result = false;
        break;
    }

    return result;
}
