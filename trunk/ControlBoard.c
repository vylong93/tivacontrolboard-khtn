/*
 * ControlBoard.c
 *
 *  Created on: Jul 31, 2014
 *      Author: MasterE
 */

#ifndef CONTROLBOARD_C_
#define CONTROLBOARD_C_

#include "ControlBoard.h"

//*****************************************************************************
// Global system tick counter holds elapsed time since the application started
//*****************************************************************************
extern uint32_t g_ui32SysTickCount;

extern uint32_t g_USBTxState;
extern uint32_t g_USBRxState;
extern uint8_t usbBufferHostToDevice[];
extern uint8_t usbBufferDeviceToHost[];

eProtocol g_eCurrentProtocol = PROTOCOL_NORMAL;

static uint32_t g_ui32TxAddress = RF_DESTINATION_ADDR;

void configureRF(void)
{
	uint8_t ui8AddrWidth;
	uint32_t ui32RxAddr;

	uint8_t OutputPowerTable[] = {
		  TI_CCxxx0_OUTPUT_POWER_1DB,
		  TI_CCxxx0_OUTPUT_POWER_0DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_2DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_4DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_6DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_8DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_10DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_12DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_14DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_16DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_18DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_20DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_22DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_24DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_26DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_28DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_30DB,
		  TI_CCxxx0_OUTPUT_POWER_MINUS_55DB
	};

	uint8_t paTable[] = { TI_CCxxx0_OUTPUT_POWER_0DB }; // Table 31 - Page 47 of 89
	uint8_t paTableLen = 1;

	RfSetChannel(usbBufferHostToDevice[1]);

	paTable[0] = OutputPowerTable[usbBufferHostToDevice[2]];

	RfWriteBurstReg(TI_CCxxx0_PATABLE, paTable, paTableLen);

	ui8AddrWidth = usbBufferHostToDevice[3];

	g_ui32TxAddress = construct4Byte(&usbBufferHostToDevice[4]);

	ui32RxAddr = construct4Byte(&usbBufferHostToDevice[4 + ui8AddrWidth]);

	Network_setSelfAddress(ui32RxAddr);

	sendResponeToHost(CONFIGURE_RF_OK);
}

void transmitDataToRobot(void)
{
	int32_t numberOfTransmittedBytes;
	numberOfTransmittedBytes = usbBufferHostToDevice[1];

//	uint32_t delayTimeBeforeSendResponeToPC;
//	delayTimeBeforeSendResponeToPC = usbBufferHostToDevice[2];

	if (numberOfTransmittedBytes > MAX_ALLOWED_DATA_LENGTH)
	{
		signalUnhandleError();
		return;
	}

	// broadcast the command and the data length to robot
	if(Network_sendMessage(g_ui32TxAddress, &usbBufferHostToDevice[3], numberOfTransmittedBytes, false))
	{
		sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_DONE);
	}
	else
	{
		sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_FAILED);
	}
}

void broadcastBslData(void)
{
	int32_t numberOfTransmittedBytes;
	uint32_t delayTimeBeforeSendResponeToPC;

	numberOfTransmittedBytes = usbBufferHostToDevice[1];
	delayTimeBeforeSendResponeToPC = usbBufferHostToDevice[2];

	if (numberOfTransmittedBytes > MAX_ALLOWED_DATA_LENGTH)
	{
		signalUnhandleError();
		return;
	}

	// Construct bootload program packet
	uint32_t i;
	usbBufferHostToDevice[0] = numberOfTransmittedBytes;
	for (i = 1; i < (numberOfTransmittedBytes + 1); i++)
	{
		usbBufferHostToDevice[i] = usbBufferHostToDevice[2 + i];
	}

	if(RfSendPacket(usbBufferHostToDevice, numberOfTransmittedBytes + 1))
	{
		g_ui32SysTickCount = 0;

		while (g_ui32SysTickCount < delayTimeBeforeSendResponeToPC);

		sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_DONE);
	}
	else
	{
		sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_FAILED);
	}
}

void receiveDataFromRobot(bool isHaveCommand)
{
	uint32_t dataLength = construct4Byte(&usbBufferHostToDevice[1]);
	uint32_t waitTime = construct4Byte(&usbBufferHostToDevice[5]);

	uint32_t i;
	uint32_t ui32MessageSize;
	uint8_t* pui8RxBuffer = 0;
	uint32_t ui32DataPointer;

	if (isHaveCommand)
	{
		uint8_t comandLength = usbBufferHostToDevice[9];

		// Transfer the command and the data length to robot, require ack
		if(!Network_sendMessage(g_ui32TxAddress, &usbBufferHostToDevice[10], comandLength, true))
		{
			usbBufferDeviceToHost[32] = RECEIVE_DATA_FROM_ROBOT_ERROR;
			USB_sendData(0);
			return;
		}
	}

	turnOnLED(LED_BLUE);

	g_ui32SysTickCount = 0;

	while (g_ui32SysTickCount < waitTime)
	{
		if (TI_CC_IsInterruptPinAsserted())
		{
			TI_CC_ClearIntFlag();

			if (Network_receivedMessage(&pui8RxBuffer, &ui32MessageSize))
			{
				turnOffLED(LED_BLUE);

				if (dataLength == ui32MessageSize)
				{
					// send received message to PC via USB
					ui32DataPointer = 0;

					while(ui32DataPointer < ui32MessageSize)
					{
						for(i = 0; i < 32 && ui32DataPointer < ui32MessageSize; i++)
						{
							usbBufferDeviceToHost[i] = pui8RxBuffer[ui32DataPointer++];
						}

						// Ready to receive data from PC
						g_USBRxState = USB_RX_IDLE;

						// Set the error byte to zero
						usbBufferDeviceToHost[32] = 0;

						// Send the received data to PC
						USB_sendData(0);

						// Is all data transmitted
						if(ui32DataPointer >= ui32MessageSize)
							break;

						// Wait for the PC to be ready to receive the next data
						while (g_USBRxState != USB_RX_DATA_AVAILABLE);

						// Did we receive the right signal?
						if (usbBufferHostToDevice[0] != RECEIVE_DATA_FROM_ROBOT_CONTINUE)
						{
							signalUnhandleError();
							return;
						}
					}

					//
					// Clean up dynamic memory maybe allocated
					//
					if (pui8RxBuffer != 0) {
						Network_deleteBuffer(pui8RxBuffer);
						pui8RxBuffer = 0;
					}
					return;
				}
			}
			//
			// Clean up dynamic memory maybe allocated
			//
			if (pui8RxBuffer != 0) {
				Network_deleteBuffer(pui8RxBuffer);
				pui8RxBuffer = 0;
			}
		}
	}
	// Signal to the PC we failed to read data from robot
	// The error signal is at the index 32 since the data read
	// from robot will be in the range [0:31]
	usbBufferDeviceToHost[32] = RECEIVE_DATA_FROM_ROBOT_ERROR;
	USB_sendData(0);
}

void scanJammingSignal(void)
{
	uint32_t waitTime = construct4Byte(&usbBufferHostToDevice[6]);

	GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);

	// Signal to the PC we failed to read data from robot
	// The error signal is at the index 32 since the data read
	// from robot will be in the range [0:31]
	usbBufferDeviceToHost[32] = RECEIVE_DATA_FROM_ROBOT_ERROR;

	g_ui32SysTickCount = 0;
	// Waiting to see any JAMMING signal from the robots
	while (true)
	{
		//TODO: Reconfig GPO2 and use CCA detect
		if (TI_CC_IsInterruptPinAsserted())			// Is JAMMING detected?
		{
			TI_CC_ClearIntFlag();

			GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);

			RfFlushRxFifo();

			TI_CC_Strobe(TI_CCxxx0_SRX);      // Initialize CCxxxx in RX mode.

			// Set the error byte to zero - indicate Rx asserted!!!
			usbBufferDeviceToHost[32] = 0;

			break; // Jamming break in half
		}

		if(g_ui32SysTickCount >= waitTime)
				break; // Success break
	}

	// Ready to receive data from PC
	g_USBRxState = USB_RX_IDLE;

	// Send the received data to PC
	USB_sendData(0);
	return;
}

#endif /* CONTROLBOARD_C_ */
