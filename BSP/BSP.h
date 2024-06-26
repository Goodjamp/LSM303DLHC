#ifndef __BSP_H__
#define __BSP_H__

/**************************I2C TARGET************/
#define LSM303DLHC_I2C                   I2C1

/**************************DMA TARGET************/

// LSM303DLHC I2C
#define LSM303DLHC_I2C_DMA                         DMA1

#define LSM303DLHC_I2C_TX_DMA_STREAM               LL_DMA_STREAM_1
#define LSM303DLHC_I2C_TX_DMA_STREAM_IRQ           DMA1_Stream1_IRQn
#define LSM303DLHC_I2C_TX_DMA_STREAM_IRQHandler    DMA1_Stream1_IRQHandler
#define LSM303DLHC_I2C_TX_DMA_CHANNEL              LL_DMA_CHANNEL_0

#define LSM303DLHC_I2C_RX_DMA_STREAM               LL_DMA_STREAM_0
#define LSM303DLHC_I2C_RX_DMA_STREAM_IRQ           DMA1_Stream0_IRQn
#define LSM303DLHC_I2C_RX_DMA_STREAM_IRQHandler    DMA1_Stream0_IRQHandler
#define LSM303DLHC_I2C_RX_DMA_CHANNEL              LL_DMA_CHANNEL_1

/**************************EXTERNAL INTERRUPT TARGET***********/

#define LSM303DLHC_DRDY_IRQ               EXTI9_5_IRQn
#define LSM303DLHC_DRDY_IRQHandler        EXTI9_5_IRQHandler
#define LSM303DLHC_DRDY_EXT_LINE          LL_EXTI_LINE_7
#define LSM303DLHC_DRDY_SYS_PORT          LL_SYSCFG_EXTI_PORTB
#define LSM303DLHC_DRDY_SYS_LINE          LL_SYSCFG_EXTI_LINE7

/**************************GPIO TARGET***********/

// LSM303DLHC I2C
#define LSM303DLHC_I2C_GPIO_PORT         GPIOB
#define LSM303DLHC_I2C_GPIO_SCL_PIN      LL_GPIO_PIN_8
#define LSM303DLHC_I2C_GPIO_SDA_PIN      LL_GPIO_PIN_9

// LSM303DLHC DRDY
#define LSM303DLHC_DRDY_GPIO_PORT         GPIOB
#define LSM303DLHC_DRDY_GPIO_PIN          LL_GPIO_PIN_7

// DEBUG
#define DEBUG_1_GPIO_PORT                GPIOA
#define DEBUG_1_GPIO_PIN                 LL_GPIO_PIN_15
#define DEBUG_2_GPIO_PORT                GPIOB
#define DEBUG_2_GPIO_PIN                 LL_GPIO_PIN_3
#define DEBUG_3_GPIO_PORT                GPIOB
#define DEBUG_3_GPIO_PIN                 LL_GPIO_PIN_1
#define DEBUG_4_GPIO_PORT                GPIOB
#define DEBUG_4_GPIO_PIN                 LL_GPIO_PIN_10
#define DEBUG_5_GPIO_PORT                GPIOB
#define DEBUG_5_GPIO_PIN                 LL_GPIO_PIN_12
#define DEBUG_6_GPIO_PORT                GPIOB
#define DEBUG_6_GPIO_PIN                 LL_GPIO_PIN_13

#endif // __BSP_H__
