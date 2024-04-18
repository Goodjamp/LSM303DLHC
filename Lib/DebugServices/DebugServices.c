#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "services.h"

#include "DebugServices.h"
#include "RingBuff.h"

#include "BSP.h"

#ifdef DEBUG_SERVICES_ENABLE_PIN
static void debugServicesInitPin(void);

#endif
#ifdef DEBUG_SERVICES_ENABLE_LOG
static int32_t debugServicesInitLog(void);

#endif
#ifdef DEBUG_SERVICES_ENABLE_TIME_MES
static debugServicesGetTimeCb getTimeCb = NULL;
#endif

int32_t debugServicesInit(debugServicesGetTimeCb getTime)
{
    int32_t result = DEBUG_SERVICES_OK;

#ifdef DEBUG_SERVICES_ENABLE_TIME_MES
    getTimeCb = getTime;
#else
    (void)getTime;
#endif

#ifdef DEBUG_SERVICES_ENABLE_PIN
    debugServicesInitPin();
#endif

#ifdef DEBUG_SERVICES_ENABLE_LOG
    if ((result = debugServicesInitLog()) != DEBUG_SERVICES_OK) {
        return result;
    }
#endif

    return result;
}

#ifdef DEBUG_SERVICES_ENABLE_PIN

static struct {
    uint32_t pin;
    GPIO_TypeDef *port;
} debugServicesPins[] = {
#if defined(DEBUG_1_GPIO_PORT) && defined(DEBUG_1_GPIO_PIN)
    [DebugPin1] = {.port = DEBUG_1_GPIO_PORT,
                   .pin = DEBUG_1_GPIO_PIN},
#endif
#if defined(DEBUG_2_GPIO_PORT) && defined(DEBUG_2_GPIO_PIN)
    [DebugPin2] = {.port = DEBUG_2_GPIO_PORT,
                   .pin = DEBUG_2_GPIO_PIN},
#endif
#if defined(DEBUG_3_GPIO_PORT) && defined(DEBUG_3_GPIO_PIN)
    [DebugPin3] = {.port = DEBUG_3_GPIO_PORT,
                   .pin = DEBUG_3_GPIO_PIN},
#endif
#if defined(DEBUG_4_GPIO_PORT) && defined(DEBUG_4_GPIO_PIN)
    [DebugPin4] = {.port = DEBUG_4_GPIO_PORT,
                   .pin = DEBUG_4_GPIO_PIN},
#endif
#if defined(DEBUG_5_GPIO_PORT) && defined(DEBUG_5_GPIO_PIN)
    [DebugPin5] = {.port = DEBUG_5_GPIO_PORT,
                   .pin = DEBUG_5_GPIO_PIN},
#endif
#if defined(DEBUG_6_GPIO_PORT) && defined(DEBUG_6_GPIO_PIN)
    [DebugPin6] = {.port = DEBUG_6_GPIO_PORT,
                   .pin = DEBUG_6_GPIO_PIN},
#endif
#if defined(DEBUG_7_GPIO_PORT) && defined(DEBUG_7_GPIO_PIN)
    [DebugPin7] = {.port = DEBUG_7_GPIO_PORT,
                   .pin = DEBUG_7_GPIO_PIN},
#endif
#if defined(DEBUG_8_GPIO_PORT) && defined(DEBUG_8_GPIO_PIN)
    [DebugPin7] = {.port = DEBUG_7_GPIO_PORT,
                   .pin = DEBUG_7_GPIO_PIN},
#endif
#if defined(DEBUG_9_GPIO_PORT) && defined(DEBUG_9_GPIO_PIN)
    [DebugPin7] = {.port = DEBUG_7_GPIO_PORT,
                   .pin = DEBUG_7_GPIO_PIN},
#endif
#if defined(DEBUG_10_GPIO_PORT) && defined(DEBUG_10_GPIO_PIN)
    [DebugPin7] = {.port = DEBUG_7_GPIO_PORT,
                   .pin = DEBUG_7_GPIO_PIN},
#endif
};

static void debugServicesInitPin(void)
{
    LL_GPIO_InitTypeDef gpioInit = {
        .Mode = LL_GPIO_MODE_OUTPUT,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
    };

    for (uint32_t k = 0;
         k < sizeof(debugServicesPins) / sizeof(debugServicesPins[0]);
         k++) {
        gpioInit.Pin = debugServicesPins[k].pin;
        if (servicesEnablePerephr(debugServicesPins[k].port) == false) {
            return;
        }
        LL_GPIO_Init(debugServicesPins[k].port, &gpioInit);
    }
}

void debugServicesPinsTest(void)
{
    bool state = true;

    for (uint32_t i = 0; i < 10; i++) {
        for (uint32_t k = 0;
             k < sizeof(debugServicesPins) / sizeof(debugServicesPins[0]);
             k++) {
            if (state) {
                debugServicesPinSet(k);
            } else {
                debugServicesPinClear(k);
            }
        }
        state = !state;
        vTaskDelay(10);
    }
}

void debugServicesPinSet(ServicesPin pin)
{
    LL_GPIO_SetOutputPin(debugServicesPins[pin].port, debugServicesPins[pin].pin);
}

void debugServicesPinClear(ServicesPin pin)
{
    LL_GPIO_ResetOutputPin(debugServicesPins[pin].port, debugServicesPins[pin].pin);
}

#else

void debugServicesPinSet(ServicesPin pin)
{
    (void)pin;
}

void debugServicesPinClear(ServicesPin pin)
{
    (void)pin;
}

#endif

#ifdef DEBUG_SERVICES_ENABLE_TIME_MES

void debugServicesStartMesTime(TimeMesH *timeMes)
{
    if (getTimeCb == NULL) {
        return;
    }
    *timeMes = getTimeCb();
}

uint32_t debugServicesGetTimeInterval(TimeMesH *timeMes)
{
    if (getTimeCb == NULL) {
        return 0;
    }
    return getTimeCb() - (*timeMes);
}

#else

void debugServicesStartMesTime(TimeMesH *timeMes)
{
}

uint32_t debugServicesGetTimeInterval(TimeMesH *timeMes)
{
    return 0;
}

#endif

#ifdef DEBUG_SERVICES_ENABLE_STATISTIC

#define STAT_MAX_INDEX    (DEBUG_SERVICES_STAT_SIZE_BUFF - 1)

void debugServicesFlowStatistic(bool isSuccess, FlowStatisticH *statIn)
{
    if (statIn->buff[statIn->pos]) {
        if (statIn->sucCnt) {
            statIn->sucCnt--;
        }
    } else {
        if (statIn->errCnt) {
            statIn->errCnt--;
        }
    }
    if (isSuccess) {
        statIn->sucCnt++;
    } else {
        statIn->errCnt++;
    }
    statIn->buff[statIn->pos] = isSuccess;
    statIn->pos = (statIn->pos + 1) & STAT_MAX_INDEX;
}

#else

void debugServicesFlowStatistic(bool isSuccess, FlowStatisticH *statIn)
{
    (void)isSuccess;
    (void)statIn;
}

#endif

#ifdef DEBUG_SERVICES_ENABLE_LOG

static const struct {
    uint32_t flexComId;
    USART_Type *usart;
    uint8_t portTx;
    uint8_t pinTx;
    uint32_t ioconModeTx;
    uint8_t portRx;
    uint8_t pinRx;
    uint32_t ioconModeRx;
    uint32_t dmaChTx;
    uint32_t dmaChRx;
    clock_attach_id_t clockSrc;
} debugPortConf = {
    .flexComId = 5,
    .usart = USART5,
    .portTx = 0,
    .pinTx = 9,
    .ioconModeTx = IOCON_FUNC3 | IOCON_MODE_INACT | IOCON_SLEW_STANDARD | IOCON_DIGITAL_EN,
    .portRx = 0,
    .pinRx = 8,
    .ioconModeTx = IOCON_FUNC3 | IOCON_MODE_INACT | IOCON_SLEW_STANDARD | IOCON_DIGITAL_EN,
    .dmaChTx = 15,
    .dmaChRx = 14,
    .clockSrc = kPLL0_DIV_to_FLEXCOMM5,
};
static struct {
    usart_dma_handle_t dmaH;
    dma_handle_t txDmaH;
    dma_handle_t rxDmaH;
} debugUsartDmaH;

#define DEBUG_USART_BASE_DMA              DMA0
#define UARTE_BR                          230400
#define LOG_MUTEX_TIMEOUTE                1000
#define TX_COMPLETE_MAX_WAITE_TIME_MS     200
#define DEBUG_SERVICES_RING_BUFF_DEPTH    5

RING_BUFF_CREATE_BUFF(logBuff, DEBUG_SERVICES_MAX_LOG_MESSAGE_SIZE,
                      DEBUG_SERVICES_RING_BUFF_DEPTH)
static RingBuffH logRingBuff;
static SemaphoreHandle_t logMutex = NULL;
static char txMessageBuff[DEBUG_SERVICES_MAX_LOG_MESSAGE_SIZE + 1];
static volatile bool isTxInProcess = false;
static status_t intStatus = kStatus_Success;

static void debugServicesCb(USART_Type *base,
                            usart_dma_handle_t *handle,
                            status_t status,
                            void *userData)
{
    uint32_t txDataSize = 0;

    if (status != kStatus_USART_TxIdle) {
        return;
    }

    if (ringBuffGetCnt(&logRingBuff) != 0) {
        ringBuffPop(&logRingBuff, (uint8_t *)txMessageBuff, (RingBuffSizeT *)&txDataSize);
        if (txDataSize != 0) {
            usart_transfer_t xfer = {
                .data = (uint8_t *)txMessageBuff,
                .dataSize = txDataSize,
            };
            USART_TransferSendDMA(debugPortConf.usart, &debugUsartDmaH.dmaH, &xfer);

            isTxInProcess = true;
            return;
        }
    }
    isTxInProcess = false;
}

static uint32_t debugServicesLogSend(char message[], uint32_t messageSize)
{
    usart_transfer_t xfer = {
        .data = (uint8_t *)message,
        .dataSize = messageSize,
    };

    isTxInProcess = true;
    if ((intStatus = USART_TransferSendDMA(debugPortConf.usart, &debugUsartDmaH.dmaH, &xfer))
        != kStatus_Success) {
        return DEBUG_SERVICES_LOG_INTERNAL_ERROR;
    }

    return DEBUG_SERVICES_OK;
}

static int32_t debugServicesInitLog(void)
{
    uint32_t flexComFrq;
    const usart_config_t usartConfig = {
        .baudRate_Bps = UARTE_BR,
        .parityMode = kUSART_ParityDisabled,
        .stopBitCount = kUSART_OneStopBit,
        .bitCountPerChar = kUSART_8BitsPerChar,
        .loopback = false,
        .enableRx = true,
        .enableTx = true,
        .enableMode32k = false,
        .txWatermark = kUSART_TxFifo0,
        .rxWatermark = kUSART_RxFifo1,
        .syncMode = kUSART_SyncModeDisabled,
        .enableContinuousSCLK = false,
        .clockPolarity = kUSART_RxSampleOnFallingEdge,
        .enableHardwareFlowControl = false,
    };

    logMutex = xSemaphoreCreateMutex();
    if (logMutex == NULL) {
        return DEBUG_SERVICES_LOG_INIT_ERROR;
    }
    ringBuffInit(&logRingBuff, logBuff,
                 DEBUG_SERVICES_MAX_LOG_MESSAGE_SIZE,
                 DEBUG_SERVICES_RING_BUFF_DEPTH, NULL);

    /*Configure USART*/
    CLOCK_AttachClk(debugPortConf.clockSrc);
    IOCON_PinMuxSet(IOCON, debugPortConf.portTx, debugPortConf.pinTx, debugPortConf.ioconModeTx);
    IOCON_PinMuxSet(IOCON, debugPortConf.portRx, debugPortConf.pinRx, debugPortConf.ioconModeRx);

    flexComFrq = CLOCK_GetFlexCommClkFreq(debugPortConf.flexComId);
    if ((intStatus =
             USART_Init(debugPortConf.usart, &usartConfig, flexComFrq)) != kStatus_Success) {
        return DEBUG_SERVICES_LOG_INTERNAL_ERROR;
    }

    DMA_EnableChannel(DEBUG_USART_BASE_DMA, debugPortConf.dmaChTx);
    DMA_EnableChannel(DEBUG_USART_BASE_DMA, debugPortConf.dmaChRx);

    DMA_CreateHandle(&debugUsartDmaH.txDmaH, DEBUG_USART_BASE_DMA, debugPortConf.dmaChTx);
    DMA_CreateHandle(&debugUsartDmaH.rxDmaH, DEBUG_USART_BASE_DMA, debugPortConf.dmaChRx);

    if ((intStatus = USART_TransferCreateHandleDMA(debugPortConf.usart,
                                                   &debugUsartDmaH.dmaH,
                                                   debugServicesCb,
                                                   NULL,
                                                   &debugUsartDmaH.txDmaH,
                                                   &debugUsartDmaH.rxDmaH)) != kStatus_Success) {
        return DEBUG_SERVICES_LOG_INTERNAL_ERROR;
    }


    return DEBUG_SERVICES_OK;
}

int32_t debugServicesLogWriteRaw(const char *message)
{
    uint32_t result = DEBUG_SERVICES_OK;

    if (strlen(message) > (DEBUG_SERVICES_MAX_LOG_MESSAGE_SIZE + 1)) {
        return DEBUG_SERVICES_LOG_MESS_SIZE_ERROR;
    }

    /*
     * We send it over the special message ring buffer
     */
    if (systemRtosSemaphoreTake(logMutex, LOG_MUTEX_TIMEOUTE) == pdFALSE) {
        return DEBUG_SERVICES_LOG_TAKE_MUTEX_ERROR;
    }
    DMA_DisableChannelInterrupts(DEBUG_USART_BASE_DMA, debugPortConf.dmaChTx);

    if (ringBuffGetCnt(&logRingBuff) == 0 && isTxInProcess == false) {
        isTxInProcess = true;
        memcpy(txMessageBuff, message, strlen(message) + 1);
        result = debugServicesLogSend(txMessageBuff, strlen(txMessageBuff));
    } else {
        ringBuffPush(&logRingBuff, (uint8_t *)message, strlen(message) + 1);
    }
    DMA_EnableChannelInterrupts(DEBUG_USART_BASE_DMA, debugPortConf.dmaChTx);
    systemRtosSemaphoreGive(logMutex);

    return result;
}

int32_t debugServicesLogWrite(DebugServicesLogType type, const char *message)
{
    char logEntry[DEBUG_SERVICES_MAX_LOG_MESSAGE_SIZE + 1];

    /* We have X bytes in the log entry in total. One byte is reserved for terminating null. Size of
     * the header is Y bytes and we must subtract one from it to exclude its own null. */
    static const uint8_t maxMessagLengthOnOneLine = sizeof(logEntry) - 1
                                                    - (sizeof(DEBUG_SERVICES_MESS_HEADR) - 1);
    TickType_t timeExpiredSinceStartMs;
    uint8_t timeStampHours;
    uint8_t timeStampMinutes;
    uint8_t timeStampSeconds;
    uint16_t timeStampMilliseconds;
    uint32_t result = DEBUG_SERVICES_OK;
    uint8_t messageLength;

    timeExpiredSinceStartMs = xTaskGetTickCount() * pdMS_TO_TICKS(1);

    timeStampHours = (timeExpiredSinceStartMs / (60 * 60 * 1000)) % 100;
    timeExpiredSinceStartMs = timeExpiredSinceStartMs % (60 * 60 * 1000);

    timeStampMinutes = timeExpiredSinceStartMs / (60 * 1000);
    timeExpiredSinceStartMs = timeExpiredSinceStartMs % (60 * 1000);

    timeStampSeconds = timeExpiredSinceStartMs / (1000);
    timeStampMilliseconds = timeExpiredSinceStartMs % (1000);

    messageLength = strlen(message);
    if (messageLength > maxMessagLengthOnOneLine) {
        messageLength = maxMessagLengthOnOneLine;
    }

    /*
     * We send it over the special message ring buffer
     */
    if (systemRtosSemaphoreTake(logMutex, LOG_MUTEX_TIMEOUTE) == pdFALSE) {
        return DEBUG_SERVICES_LOG_TAKE_MUTEX_ERROR;
    }
    DMA_DisableChannelInterrupts(DEBUG_USART_BASE_DMA, debugPortConf.dmaChTx);

    sprintf(logEntry,
            "%02u:%02u:%02u.%02u - %c - ",
            timeStampHours, timeStampMinutes, timeStampSeconds, timeStampMilliseconds, type);
    strncat(logEntry, message, messageLength);
    strcat(logEntry, "\n\r");

    if (ringBuffGetCnt(&logRingBuff) == 0
        && isTxInProcess == false) {
        isTxInProcess = true;
        memcpy(txMessageBuff, logEntry, strlen(logEntry) + 1);
        result = debugServicesLogSend(txMessageBuff, strlen(txMessageBuff));
    } else {
        ringBuffPush(&logRingBuff, (uint8_t *)logEntry, strlen(logEntry) + 1);
    }
    DMA_EnableChannelInterrupts(DEBUG_USART_BASE_DMA, debugPortConf.dmaChTx);
    systemRtosSemaphoreGive(logMutex);

    return result;
}

#else

int32_t debugServicesLogWrite(DebugServicesLogType type, const char *message)
{
    (void)type;
    (void)message;

    return DEBUG_SERVICES_OK;
}

#endif
