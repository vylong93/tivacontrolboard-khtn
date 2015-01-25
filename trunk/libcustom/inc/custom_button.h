/*
 * custom_button.h
 *
 *  Created on: Dec 17, 2014
 *      Author: VyLong
 */

#ifndef CUSTOM_BUTTON_H_
#define CUSTOM_BUTTON_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

//*****************************************************************************
//
// Configure SW1 input at PF4 and interrupt enable
//
//*****************************************************************************
void initLaunchpadSW1(void);

void LaunchpadButtonIntHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_BUTTON_H_ */
