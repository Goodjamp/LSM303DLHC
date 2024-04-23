#ifndef __SERVICES_H__
#define __SERVICES_H__

#include <stdint.h>
#include <stdbool.h>

bool servicesEnablePerephr(void *pereph);
bool servicesDisablePerephr(void *pereph);
bool servicesSetResetPerephr(void *pereph);
bool servicesClearResetPerephr(void *pereph);

#endif
