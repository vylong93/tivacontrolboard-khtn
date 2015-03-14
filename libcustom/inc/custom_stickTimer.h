/*
 * custom_stickTimer.h
 *
 *  Created on: Feb 6, 2015
 *      Author: VyLong
 */

#ifndef CUSTOM_STICKTIMER_H_
#define CUSTOM_STICKTIMER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

//*****************************************************************************
// The system tick timer period.
//*****************************************************************************
#define SYSTICKS_PER_SECOND     1000

//*****************************************************************************
// SysTickIntHandler
//*****************************************************************************
void initSysTick(void);
void resetTickCounter(void);
uint32_t getTickCounterValue(void);
void SysTickHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_STICKTIMER_H_ */
