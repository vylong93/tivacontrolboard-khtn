#include "ControlBoard.h"

extern volatile bool g_bSuspended;
extern volatile bool g_bConnected;
extern uint32_t g_USBTxState;
extern uint32_t g_USBRxState;
extern uint8_t usbBufferHostToDevice[];
extern uint8_t usbBufferDeviceToHost[];
extern char g_BluetoothBuffer[];
extern uint8_t g_ui8BluetoothCounter;
extern eProtocol g_eCurrentProtocol;
uint32_t
convertByteToUINT32(uint8_t data[]);

void TI_CC_IRQ_handler(void);
void TI_CC_IRQ_handler(void){}

void main(void)
{
	initSystem();

	while (1)
	{
		if ((g_bConnected == false) || (g_bSuspended == true))
		{
			GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);
			continue;
		}

		GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_GREEN);

		if (g_USBRxState == USB_RX_DATA_AVAILABLE)
		{
			GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);
			switch (usbBufferHostToDevice[0])
			{
			case CONFIGURE_BOOTLOAD_PROTOCOL:
				g_eCurrentProtocol = PROTOCOL_BOOTLOAD;
				sendResponeToHost(CONFIGURE_BOOTLOAD_PROTOCOL_OK);
				break;

			case CONFIGURE_NORMAL_PROTOCOL:
				g_eCurrentProtocol = PROTOCOL_NORMAL;
				sendResponeToHost(CONFIGURE_NORMAL_PROTOCOL_OK);
				break;

			//---------------------- out of date S ----------------------
			case CONFIGURE_SPI:
				configureSPI();
				break;

			case CONFIGURE_RF:
				configureRF();
				break;

			case TRANSMIT_DATA_TO_ROBOT:
				switch (g_eCurrentProtocol)
				{
				case PROTOCOL_BOOTLOAD:
					broadcastBslData();
					break;

				default:	// PROTOCOL_NORMAL
					transmitDataToRobot();
					break;
				}
				break;

			case RECEIVE_DATA_FROM_ROBOT:
				switch (g_eCurrentProtocol)
				{
				case PROTOCOL_BOOTLOAD:
					scanJammingSignal();
					break;

				default:	// PROTOCOL_NORMAL
					receiveDataFromRobot(false);
					break;
				}
				break;

			case RECEIVE_DATA_FROM_ROBOT_COMMAND:
				// PROTOCOL_NORMAL
				receiveDataFromRobot(true);
				break;
			//---------------------- out of date E ----------------------

			default:
				signalUnhandleError();
				break;
			}
			g_USBRxState = USB_RX_IDLE;
			GPIOPinWrite(LED_PORT_BASE, LED_BLUE, 0);
		}
	}
}

//*****************************************************************************
// Convert 4 bytes of a array to uint32
// @param data[]: The converted byte array
// @return : a uint32 value.
//*****************************************************************************
uint32_t convertByteToUINT32(uint8_t data[])
{
	uint32_t result = data[0];
	result <<= 8;
	result |= data[1];
	result <<= 8;
	result |= data[2];
	result <<= 8;
	result |= data[3];
	return result;
}

void BluetoothIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART2_BASE, true); //get interrupt status
	UARTIntClear(UART2_BASE, ui32Status); //clear the asserted interrupts

	while (UARTCharsAvail(UART2_BASE)) //loop while there are chars
	{
		ui32Status = UARTCharGetNonBlocking(UART2_BASE);

		if ((ui32Status & 0x00000F00) == 0)
		{
			g_BluetoothBuffer[g_ui8BluetoothCounter++] = ui32Status;
			g_ui8BluetoothCounter =
					(g_ui8BluetoothCounter >= BLUETOOTH_BUFFER_SIZE) ?
							(0) : (g_ui8BluetoothCounter);
		}

		GPIOPinWrite(LED_PORT_BASE, LED_BLUE, LED_BLUE); //blink LED
		SysCtlDelay(SysCtlClockGet() / (1000 * 3)); //delay ~1 msec
		GPIOPinWrite(LED_PORT_BASE, LED_BLUE, 0); //turn off LED
	}

	if (g_ui8BluetoothCounter > 2
			&& g_BluetoothBuffer[g_ui8BluetoothCounter - 2] == 0x0D
			&& g_BluetoothBuffer[g_ui8BluetoothCounter - 1] == 0x0A) // Is detected /r/n endlinechar?
	{
		if (g_BluetoothBuffer[0] == SMART_PHONE_REQUEST_CONFIG)
		{
			// RF24_TX_setAddress((unsigned char *)&g_BluetoothBuffer[1]);

			UARTCharPut(UART2_BASE, 'O');
			UARTCharPut(UART2_BASE, 'K');
			UARTCharPut(UART2_BASE, ' ');
			UARTCharPut(UART2_BASE, (g_BluetoothBuffer[1] >> 4) + '0');
			UARTCharPut(UART2_BASE, (g_BluetoothBuffer[1] & 0xF) + '0');
			UARTCharPut(UART2_BASE, '\r');
			UARTCharPut(UART2_BASE, '\n');
		}
		else
		{
			g_ui8BluetoothCounter -= 2;
			// send to RF
//			RF24_TX_activate();
//			RF24_TX_writePayloadNoAck((unsigned char)(g_ui8BluetoothCounter), (unsigned char*)g_BluetoothBuffer);
//			RF24_TX_pulseTransmit();
//
//			while (GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) != 0)
//				;
//
//			RF24_clearIrqFlag(RF24_IRQ_MASK);
//
//			RF24_RX_activate();
		}

		GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_ALL); //blink LED
		SysCtlDelay(SysCtlClockGet() / (1000 * 3)); //delay ~1 msec
		GPIOPinWrite(LED_PORT_BASE, LED_ALL, 0); //turn off LED

		// clear bluetooth buffer
		for(ui32Status = 0; ui32Status < BLUETOOTH_BUFFER_SIZE; ui32Status++)
			g_BluetoothBuffer[ui32Status] = 0;
		g_ui8BluetoothCounter = 0;
	}
}

void LaunchpadButtonIntHandler(void)
{
//	// Testing only
//
//	RF24_TX_activate();
//
//	SysCtlDelay(533333);  // ~100ms debound
//
//	unsigned char buff = 0xFC; 					// set Distance measure command
//	RF24_TX_writePayloadNoAck(1, &buff);
//	RF24_TX_pulseTransmit();
//
//	while (GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) != 0)
//		;
//
//	if (RF24_getIrqFlag(RF24_IRQ_TX))
//	{
//		GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);
//	}
//
//	if (RF24_getIrqFlag(RF24_IRQ_MAX_RETRANS))
//	{
//		GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);
//	}
//
//	RF24_clearIrqFlag(RF24_IRQ_MASK);
//
//	SysCtlDelay(533333);  // ~100ms debound
//
//	RF24_RX_activate();
//
//	GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);

	UARTCharPut(UART2_BASE, 'V');
	UARTCharPut(UART2_BASE, 'y');
	UARTCharPut(UART2_BASE, 'L');
	UARTCharPut(UART2_BASE, 'o');
	UARTCharPut(UART2_BASE, 'n');
	UARTCharPut(UART2_BASE, 'g');
	UARTCharPut(UART2_BASE, '\r');
	UARTCharPut(UART2_BASE, '\n');

	GPIOPinWrite(LED_PORT_BASE, LED_RED, LED_RED); //blink LED
	SysCtlDelay(SysCtlClockGet() / (1000 * 3)); //delay ~1 msec
	GPIOPinWrite(LED_PORT_BASE, LED_RED, 0); //turn off LED
}
