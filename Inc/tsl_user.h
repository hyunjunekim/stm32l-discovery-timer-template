#ifndef __TSL_USER_H
#define __TSL_USER_H

#include "tsl.h"

void TSL_user_Init(void);
void TSL_user_InitGPIOs(void);
void TSL_user_SetThresholds(void);

int getTouchSliderPercent(void);
int getTouchButtons(void);

#endif
