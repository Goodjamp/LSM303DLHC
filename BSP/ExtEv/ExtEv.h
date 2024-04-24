#ifndef __EXT_EV_H__
#define __EXT_EV_H__

typedef enum {
    EXT_EV_OK,
    EXT_EV_TARGET_ERROR,
} ExtEvResult;

typedef enum {
    EXT_EV_TARGET_LSM303DLHC,
} ExtEvTarget;

typedef struct {
    void (*extEvCb)(void);
} ExtEvLsm303dlhcSettings;

typedef union {
    ExtEvLsm303dlhcSettings lsm303dlhc;
} ExtEvSettings;

ExtEvResult extEvInit(ExtEvTarget extEvTarget, ExtEvSettings settings);

#endif