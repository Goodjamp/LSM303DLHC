#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

typedef enum {
    TIMER_OK,
    TIMER_TARGET_ERROR,
} TimerResult;

typedef enum {
    TIMER_CRSF_EVENT_TIMING_1,
    TIMER_CRSF_EVENT_TIMING_2,
    TIMER_CRSF_EVENT_TIMING_3,
    TIMER_CRSF_EVENT_TIMING_4,
    TIMER_CRSF_EVENT_NUMBER,
} TimerCrsfEvent;

typedef enum {
    TIEMR_TARGET_SRSF
} TimerTarget;

typedef struct {
    void (*timerEvent)(TimerCrsfEvent event);
} TimerCrsfCb;

typedef union {
    TimerCrsfCb crsf;
} TimerCb;

typedef struct {
    uint16_t timouteList[TIMER_CRSF_EVENT_NUMBER];
} TimerCrsfSettings;

typedef union {
    TimerCrsfSettings crsf;
} TimerSettings;

TimerResult timerInit(TimerTarget timerTarget, TimerSettings settings, TimerCb cb);
TimerResult timerStart(TimerTarget timerTarget);
TimerResult timerStop(TimerTarget timerTarget);

#endif