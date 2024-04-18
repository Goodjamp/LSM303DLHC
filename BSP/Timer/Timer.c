#include <stdint.h>
#include <stdlib.h>

#include "DebugServices.h"
#include "Bsp.h"
#include "services.h"
#include "Timer.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_rcc.h"

volatile static struct {
    TimerCrsfCb crsfCb;
} cbList;
volatile static uint32_t crsfCcVal[TIMER_CRSF_EVENT_NUMBER] = {0, 0, 0, 0};

#define CRSF_TIMER_ARR    32767

#if (CRSF_TIMER_ARR & (CRSF_TIMER_ARR + 1)) != 0
#error (CRSF_TIMER_ARR + 1) must be pow2
#endif

#define CRSF_GET_NEXT_CC_VAL(STEP)    ((LL_TIM_GetCounter(CRSF_TIMER) + STEP) & CRSF_TIMER_ARR)

void TIM4_IRQHandler()
{

}

static int timerCrsfInit(const TimerCrsfSettings *settings, const TimerCrsfCb *cb)
{

    return TIMER_OK;
}

TimerResult timerInit(TimerTarget timerTarget, TimerSettings settings, TimerCb cb)
{
    TimerResult result = TIMER_OK;

    switch (timerTarget) {
    case TIEMR_TARGET_SRSF:
        result = timerCrsfInit(&settings.crsf, &cb.crsf);
        break;

    default:
        result = TIMER_TARGET_ERROR;
    }

    return result;
}

TimerResult timerStart(TimerTarget timerTarget)
{
    TimerResult result = TIMER_OK;

    switch (timerTarget) {
    case TIEMR_TARGET_SRSF:
        LL_TIM_EnableCounter(CRSF_TIMER);
        break;

    default:
        result = TIMER_TARGET_ERROR;
    }

    return result;
}

TimerResult timerStop(TimerTarget timerTarget)
{
    TimerResult result = TIMER_OK;

    switch (timerTarget) {
    case TIEMR_TARGET_SRSF:
        LL_TIM_DisableCounter(CRSF_TIMER);
        break;

    default:
        result = TIMER_TARGET_ERROR;
    }

    return result;
}