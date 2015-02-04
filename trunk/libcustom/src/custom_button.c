/*
 * custom_button.c
 *
 *  Created on: Dec 17, 2014
 *      Author: VyLong
 */

#include "libcustom\inc\custom_button.h"
#include "libcustom\inc\custom_led.h"

extern void Sw1IrqHandler();

void initLaunchpadSW1(void)
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_SysCtlDelay(2);

	// unlock the GPIO commit control register to modify PF0 configuration because it may be configured to be a NMI input.

//	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
//	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

	ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);
	ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	// Configure Interrupt
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
	ROM_GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);

	IntRegister(INT_GPIOF, LaunchpadButtonIntHandler);

	ROM_IntEnable(INT_GPIOF);
	ROM_IntPrioritySet(INT_GPIOF, 0x00);
	ROM_IntMasterEnable();
}

void LaunchpadButtonIntHandler(void)
{
	if((GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_INT_PIN_4) == GPIO_INT_PIN_4)
	{
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);

		ROM_GPIOPinWrite(LED_PORT_BASE, LED_RED, LED_RED);  	//blink LED
		ROM_SysCtlDelay(SysCtlClockGet() / (3 * 1000) * 150);  	//delay ~150 msec

		Sw1IrqHandler();

		ROM_GPIOPinWrite(LED_PORT_BASE, LED_RED, 0);  		//turn off LED
	}
}
