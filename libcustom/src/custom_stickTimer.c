/*
 * custom_stickTimer.c
 *
 *  Created on: Feb 6, 2015
 *      Author: VyLong
 */

#include "libcustom/inc/custom_stickTimer.h"


//*****************************************************************************
// Global system tick counter holds elapsed time since the application started
//*****************************************************************************
static uint32_t g_ui32SysTickCount;

void initSysTick(void)
{
	// Set the system tick to fire 1000 times per second.
	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKS_PER_SECOND);
	SysTickIntRegister(SysTickHandler);
	ROM_SysTickIntEnable();
	ROM_SysTickEnable();
}

void resetTickCounter(void)
{
	g_ui32SysTickCount = 0;
}

uint32_t getTickCounterValue(void)
{
	return g_ui32SysTickCount;
}

void SysTickHandler(void)
{
	g_ui32SysTickCount++;
}
