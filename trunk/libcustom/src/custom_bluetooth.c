/*
 * custom_bluetooth.c
 *
 *  Created on: Jan 25, 2015
 *      Author: VyLong
 */

#include "libcustom/inc/custom_led.h"
#include "libcustom/inc/custom_bluetooth.h"

uint8_t g_BluetoothBuffer[BLUETOOTH_BUFFER_SIZE];
uint8_t g_ui8BluetoothCounter = 0;

// This function only be called after bluetooth received a completed sequence command (have endline char)
// after this function return, all data in Cmd buffer will be drop
extern void BluetoothCommandDecoder(uint8_t* pui8Cmd, uint8_t ui8Length);

void initBluetooth(void)
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	// unlock the GPIO commit control register to modify PD7 configuration.
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
	HWREG(GPIO_PORTD_BASE + GPIO_O_AFSEL) &= ~0x80;
	HWREG(GPIO_PORTD_BASE + GPIO_O_DEN) |= 0x80;
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

	ROM_GPIOPinConfigure(GPIO_PD6_U2RX);
	ROM_GPIOPinConfigure(GPIO_PD7_U2TX);

	ROM_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	ROM_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7,
			GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);
	ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);

	ROM_UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	ROM_SysCtlDelay(ROM_SysCtlClockGet() / 3);

	UARTIntRegister(UART2_BASE, BluetoothIntHandler);	// Register irq function and enable interrupt

	ROM_UARTIntEnable(UART2_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts
}

void BluetoothIntHandler(void)
{
	uint32_t ui32Status;

	toggleLED(LED_ALL);

	ui32Status = ROM_UARTIntStatus(UART2_BASE, true); //get interrupt status
	ROM_UARTIntClear(UART2_BASE, ui32Status); //clear the asserted interrupts

	while (ROM_UARTCharsAvail(UART2_BASE)) //loop while there are chars
	{
		ui32Status = ROM_UARTCharGetNonBlocking(UART2_BASE);

		if ((ui32Status & 0x00000F00) == 0)
		{
			g_BluetoothBuffer[g_ui8BluetoothCounter++] = ui32Status;
			g_ui8BluetoothCounter =
					(g_ui8BluetoothCounter >= BLUETOOTH_BUFFER_SIZE) ?
							(0) : (g_ui8BluetoothCounter);
		}
	}

	// Is received completed sequences command
	if (g_ui8BluetoothCounter > 2
			&& g_BluetoothBuffer[g_ui8BluetoothCounter - 2] == 0x0D
			&& g_BluetoothBuffer[g_ui8BluetoothCounter - 1] == 0x0A) // Is detected /r/n endlinechar?
	{
		// Call decoder
		BluetoothCommandDecoder(g_BluetoothBuffer, g_ui8BluetoothCounter);

		// Clear endline char
		g_BluetoothBuffer[g_ui8BluetoothCounter - 2] = 0;
		g_BluetoothBuffer[g_ui8BluetoothCounter - 1] = 0;

		// Reset counter
		g_ui8BluetoothCounter = 0;
	}

	toggleLED(LED_ALL);
}
