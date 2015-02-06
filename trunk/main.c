#include "ControlBoard.h"

extern volatile bool g_bSuspended;
extern volatile bool g_bConnected;
extern uint32_t g_USBTxState;
extern uint32_t g_USBRxState;
extern uint8_t usbBufferHostToDevice[];
extern uint8_t usbBufferDeviceToHost[];

void Sw1IrqHandler(void);
void MCU_RF_IRQ_handler(void);
void BluetoothCommandDecoder(uint8_t* pui8Cmd, uint8_t ui8Length);

void main(void)
{
	initSysClock();

	initUartDebug();

	initLeds();

	initLaunchpadSW1();

	initSysTick();

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
			//-----------------Bootloader Handle-------------------

			case BOOTLOADER_BROADCAST_PACKET:
				broadcastBslData();
				break;

			case BOOTLOADER_SCAN_JAMMING:
				scanJammingSignal();
				break;

			//------------------Robot Communication------------------

			case TRANSMIT_DATA_TO_ROBOT:
				transmitDataToRobot();
				break;

			case TRANSMIT_DATA_TO_ROBOT_ACK:
				transmitDataToRobotWithACK();
				break;

			case RECEIVE_DATA_FROM_ROBOT:
				receiveDataFromRobot(false);
				break;

			case RECEIVE_DATA_FROM_ROBOT_COMMAND:
				receiveDataFromRobot(true);
				break;

			//---------------Configuration---------------------

			case CONFIGURE_SPI:
				configureSPI();
				break;

			case CONFIGURE_RF:
				configureRF();
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
