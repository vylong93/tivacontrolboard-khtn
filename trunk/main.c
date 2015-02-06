#include "ControlBoard.h"

extern volatile bool g_bSuspended;
extern volatile bool g_bConnected;
extern uint32_t g_USBTxState;
extern uint32_t g_USBRxState;
extern uint8_t usbBufferHostToDevice[];
extern uint8_t usbBufferDeviceToHost[];

extern eProtocol g_eCurrentProtocol;

void Sw1IrqHandler(void);
void MCU_RF_IRQ_handler(void);
void BluetoothCommandDecoder(uint8_t* pui8Cmd, uint8_t ui8Length);

void main(void)
{
	initSysClock();

	initUartDebug();

	initLeds();

	initLaunchpadSW1();

	// Set the system tick to fire 1000 times per second.
	SysTickPeriodSet(SysCtlClockGet() / SYSTICKS_PER_SECOND);
	SysTickIntEnable();
	SysTickEnable();

	initBluetooth();

	initUSB();

	initRfModule(false);

	Network_setSelfAddress(RF_CONTOLBOARD_ADDR);

	while (true)
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

			case TRANSMIT_DATA_TO_ROBOT_ACK:		// PROTOCOL_NORMAL
				//TODO: implement
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

			case RECEIVE_DATA_FROM_ROBOT_COMMAND:	// PROTOCOL_NORMAL
				receiveDataFromRobot(true);
				break;

			default:
				signalUnhandleError();
				break;
			}

			g_USBRxState = USB_RX_IDLE;

			turnOffLED(LED_BLUE);
		}
	}
}

void Sw1IrqHandler(void)
{
	// Put your testing code here!
}

void MCU_RF_IRQ_handler(void)
{
	// Make the complier happy, Interrupt is not used
}

void BluetoothCommandDecoder(uint8_t* pui8Cmd, uint8_t ui8Length)
{
//	if (pui8Cmd[0] == SMART_PHONE_REQUEST_CONFIG)
//	{
//		// RF24_TX_setAddress((unsigned char *)&g_BluetoothBuffer[1]);
//
//		UARTCharPut(UART2_BASE, 'O');
//		UARTCharPut(UART2_BASE, 'K');
//		UARTCharPut(UART2_BASE, ' ');
//		UARTCharPut(UART2_BASE, (pui8Cmd[1] >> 4) + '0');
//		UARTCharPut(UART2_BASE, (pui8Cmd[1] & 0xF) + '0');
//		UARTCharPut(UART2_BASE, '\r');
//		UARTCharPut(UART2_BASE, '\n');
//	}
}
