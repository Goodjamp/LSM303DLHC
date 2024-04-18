#ifndef __BSP_H__
#define __BSP_H__

/**************************I2C TARGET************/
#define SSD1306_I2C                  I2C1

/**************************DMA TARGET************/

// SSD1306 I2C
#define SSD1306_I2C_DMA               DMA1
#define SSD1306_I2C_DMA_STREAM        LL_DMA_STREAM_1
#define SSD1306_I2C_DMA_CHANNEL       LL_DMA_CHANNEL_0


/**************************GPIO TARGET***********/

// SSD1306 I2C
#define SSD1306_I2C_GPIO_PORT        GPIOB
#define SSD1306_I2C_GPIO_SCL_PIN     LL_GPIO_PIN_8
#define SSD1306_I2C_GPIO_SDA_PIN     LL_GPIO_PIN_9

// DEBUG
#define DEBUG_1_GPIO_PORT            GPIOA
#define DEBUG_1_GPIO_PIN             LL_GPIO_PIN_15
#define DEBUG_2_GPIO_PORT            GPIOB
#define DEBUG_2_GPIO_PIN             LL_GPIO_PIN_3
#define DEBUG_3_GPIO_PORT            GPIOB
#define DEBUG_3_GPIO_PIN             LL_GPIO_PIN_1
#define DEBUG_4_GPIO_PORT            GPIOB
#define DEBUG_4_GPIO_PIN             LL_GPIO_PIN_10
#define DEBUG_5_GPIO_PORT            GPIOB
#define DEBUG_5_GPIO_PIN             LL_GPIO_PIN_12
#define DEBUG_6_GPIO_PORT            GPIOB
#define DEBUG_6_GPIO_PIN             LL_GPIO_PIN_13

#endif // __BSP_H__
