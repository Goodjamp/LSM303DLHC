#ifndef __DEBUG_SERVICES_H__
#define __DEBUG_SERVICES_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "SEGGER_RTT.h"

#define DEBUG_SERVICES_ENABLE_PIN
//#define DEBUG_SERVICES_ENABLE_LOG
//#define DEBUG_SERVICES_ENABLE_STATISTIC
//#define DEBUG_SERVICES_ENABLE_TIME_MES

#define DEBUG_SERVICES_OK                         0
#define DEBUG_SERVICES_LOG_INIT_ERROR             -1
#define DEBUG_SERVICES_LOG_TAKE_MUTEX_ERROR       -2
#define DEBUG_SERVICES_LOG_SEND_ERROR             -3
#define DEBUG_SERVICES_LOG_SEND_TIMEOUTE_ERROR    -4
#define DEBUG_SERVICES_LOG_INTERNAL_ERROR         -5
#define DEBUG_SERVICES_LOG_MESS_SIZE_ERROR        -7

#define DEBUG_SERVICES_MESS_HEADR                 "hh:mm:ss:msm - T - \n\r"
#define DEBUG_SERVICES_STAT_SIZE_BUFF             256
#define DEBUG_SERVICES_MAX_LOG_MESSAGE_SIZE       100
#define DEBUG_SERVICES_MESS_MAX_SIZE              (DEBUG_SERVICES_MAX_LOG_MESSAGE_SIZE \
                                                   - sizeof(DEBUG_SERVICES_MESS_HEADR) + 1)

#ifdef DEBUG_SERVICES_ENABLE_LOG
#define DEBUG_SERVICES_SEND_MESS(MESS_TYPE, ...)    {                            \
        char debugStr[DEBUG_SERVICES_MESS_MAX_SIZE];                             \
        if (snprintf(debugStr, DEBUG_SERVICES_MESS_MAX_SIZE, __VA_ARGS__) > 0) { \
            debugServicesLogWrite(MESS_TYPE, debugStr);                          \
        }                                                                        \
}
#else
#define DEBUG_SERVICES_SEND_MESS(MESS_TYPE, ...)
#endif

// Enumerations order must be in sync with "TaskApplicationLogType"
typedef enum {
    DEBUG_SERVICES_LOG_TYPE_ERROR = 'E',
    DEBUG_SERVICES_LOG_TYPE_INFO = 'I',
    DEBUG_SERVICES_LOG_TYPE_WARNING = 'W'
} DebugServicesLogType;

typedef enum {
    DebugPin1,
    DebugPin2,
    DebugPin3,
    DebugPin4,
    DebugPin5,
    DebugPin6,
    DebugPin7,
    DebugPin8,
    DebugPin9,
    DebugPin10,
} ServicesPin;

typedef struct {
    bool buff[DEBUG_SERVICES_STAT_SIZE_BUFF];
    uint32_t pos;
    uint32_t errCnt;
    uint32_t sucCnt;
} FlowStatisticH;

typedef uint32_t (*debugServicesGetTimeCb)(void);
typedef uint32_t TimeMesH;

int32_t debugServicesInit(debugServicesGetTimeCb getTime);
void debugServicesPinsTest(void);
void debugServicesPinSet(ServicesPin pin);
void debugServicesPinClear(ServicesPin pin);
void debugServicesStartMesTime(TimeMesH *timeMes);
uint32_t debugServicesGetTimeInterval(TimeMesH *timeMes);
void debugServicesFlowStatistic(bool isSuccess, FlowStatisticH *statIn);
int32_t debugServicesLogWrite(DebugServicesLogType type, const char *message);
int32_t debugServicesLogWriteRaw(const char *message);

#endif
