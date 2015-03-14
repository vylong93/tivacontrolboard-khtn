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

void normalPacketHandle(void);
bool receivedUsbPacket(uint8_t** ppui8PacketBuffer, uint32_t* pui32PacketSize);
void decodeUsbPacket(uint8_t* pui8PacketBuffer, uint32_t ui32PacketSize);

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

	setRfTxAddress(RF_DESTINATION_ADDR);

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

			default:
				normalPacketHandle();
				break;
			}

			g_USBRxState = USB_RX_IDLE;

			turnOffLED(LED_BLUE);
		}
	}
}

void normalPacketHandle(void)
{
	uint32_t ui32PacketSize;
	uint8_t* pui8PacketBuffer = 0;

	if (receivedUsbPacket(&pui8PacketBuffer, &ui32PacketSize))
	{
		decodeUsbPacket(pui8PacketBuffer, ui32PacketSize);
	}

	if (pui8PacketBuffer != 0)
	{
		free(pui8PacketBuffer);
		pui8PacketBuffer = 0;
	}
}

bool receivedUsbPacket(uint8_t** ppui8PacketBuffer, uint32_t* pui32PacketSize)
{
	uint32_t i;
	uint32_t ui32DataPointer = 0;
	uint32_t ui32RemaindPacketLength;

	if(usbBufferHostToDevice[0] == USB_PACKET_SINGLE || usbBufferHostToDevice[0] == USB_PACKET_FIRST)
	{
		*pui32PacketSize = construct4Byte(&usbBufferHostToDevice[1]);
		(*ppui8PacketBuffer) = (uint8_t *) malloc((*pui32PacketSize));

		for(i = 0; i < USB_PACKET_MAX_SEGMENT_LENGTH && ui32DataPointer < (*pui32PacketSize); i++)
		{
			(*ppui8PacketBuffer)[ui32DataPointer++] = usbBufferHostToDevice[5 + i];
		}

		ui32RemaindPacketLength = (*pui32PacketSize) - ui32DataPointer;

		sendResponeToHost(USB_TRANSMIT_SEGMENT_DONE);

		while(ui32RemaindPacketLength > 0)
		{
			// Ready to receive data from PC
			setUsbRxState(USB_RX_IDLE);

			// Wait for the PC to be ready to receive the next data
			while (getUsbRxState() != USB_RX_DATA_AVAILABLE);

			if(usbBufferHostToDevice[0] == USB_PACKET_LAST || usbBufferHostToDevice[0] == USB_PACKET_MIDDLE)
			{
				for(i = 0; i < USB_PACKET_MAX_SEGMENT_LENGTH && ui32DataPointer < (*pui32PacketSize); i++)
				{
					(*ppui8PacketBuffer)[ui32DataPointer++] = usbBufferHostToDevice[1 + i];
				}

				ui32RemaindPacketLength = (*pui32PacketSize) - ui32DataPointer;

				sendResponeToHost(USB_TRANSMIT_SEGMENT_DONE);
			}
		}

		// Ready to receive data from PC
		setUsbRxState(USB_RX_IDLE);

		return true;
	}
	else
		return false;
}

void decodeUsbPacket(uint8_t* pui8PacketBuffer, uint32_t ui32PacketSize)
{
	switch (pui8PacketBuffer[0])
	{
	//---------------Configuration---------------------
	case CONFIGURE_SPI:
		configureSPI(&pui8PacketBuffer[1]);
		break;

	case CONFIGURE_RF:
		configureRF(&pui8PacketBuffer[1]);
		break;

	case CONFIGURE_RF_TX_ADDRESS:

		configureRFTxAddress(&pui8PacketBuffer[1]);
		break;

	//------------------Robot Communication------------------

	case TRANSMIT_DATA_TO_ROBOT:
		transmitMessageToRobot(pui8PacketBuffer, false);
		break;

	case TRANSMIT_DATA_TO_ROBOT_ACK:
		transmitMessageToRobot(pui8PacketBuffer, true);
		break;

	case RECEIVE_DATA_FROM_ROBOT:
		receiveDataFromRobot(pui8PacketBuffer, false);
		break;

	case RECEIVE_DATA_FROM_ROBOT_COMMAND:
		receiveDataFromRobot(pui8PacketBuffer, true);
		break;

	default:
//		signalUnhandleError();
		break;
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
