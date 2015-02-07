/*
 * ControlBoard.c
 *
 *  Created on: Jul 31, 2014
 *      Author: MasterE
 */

#ifndef CONTROLBOARD_C_
#define CONTROLBOARD_C_

#include "ControlBoard.h"

extern uint32_t g_ui32TxAddress;

extern uint8_t usbBufferHostToDevice[];
extern uint8_t usbBufferDeviceToHost[];

void setRfTxAddress(uint32_t ui32Address)
{
	g_ui32TxAddress = ui32Address;
}

uint32_t getRfTxAddress(void)
{
	return g_ui32TxAddress;
}

void configureSPI(uint8_t* pui8ConfigData)
{
//	uint32_t spiProtocol = usbBufferHostToDevice[1];
//	uint32_t spiClock = usbBufferHostToDevice[2];
//	spiClock = spiClock << 8;
//	spiClock |= usbBufferHostToDevice[3];
//	spiClock = spiClock << 8;
//	spiClock |= usbBufferHostToDevice[4];
//	spiClock = spiClock << 8;
//	spiClock |= usbBufferHostToDevice[5];
//	uint32_t spiDataWidth = usbBufferHostToDevice[6];
//	uint32_t spiMode = SSI_MODE_MASTER;
//
//	SSIDisable(RF24_SPI);
//
//	// Configure and enable the SSI port for SPI master mode.
//	SSIConfigSetExpClk(RF24_SPI, SysCtlClockGet(), spiProtocol, spiMode,
//			spiClock, spiDataWidth);
//
//	SSIEnable(RF24_SPI);

	sendResponeToHost(CONFIGURE_SPI_OK);
}

void configureRF(uint8_t* pui8ConfigData)
{
	uint8_t RF_POWER_TABLE[4] = {
			RF24_POWER_0DBM,
			RF24_POWER_MINUS6DBM,
			RF24_POWER_MINUS12DBM,
			RF24_POWER_MINUS18DBM
	};

	uint8_t ui8Channel = pui8ConfigData[0];
	RfSetChannel(ui8Channel);

	uint8_t ui8PowerIndex = pui8ConfigData[1];

	uint8_t rfSpeedAndPower = RF24_readRegister(RF24_REG_RF_SETUP);
	rfSpeedAndPower = (rfSpeedAndPower & (~RF24_POWER_MASK)) | RF_POWER_TABLE[ui8PowerIndex];
	RF24_writeRegister(RF24_REG_RF_SETUP, rfSpeedAndPower);

//	uint8_t ui8AddressWidth = pui8ConfigData[2];

	uint32_t ui32TxAddress = construct4Byte(&pui8ConfigData[3]);

	setRfTxAddress(ui32TxAddress);

	uint32_t ui32SelfAddress = construct4Byte(&pui8ConfigData[7]);

	Network_setSelfAddress(ui32SelfAddress);

	sendResponeToHost(CONFIGURE_RF_OK);
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
	for (i = 1; i <= numberOfTransmittedBytes; i++)
	{
		usbBufferHostToDevice[i] = usbBufferHostToDevice[2 + i];
	}

	if(RfSendPacket(usbBufferHostToDevice))
	{
		resetTickCounter();

		while (getTickCounterValue() < delayTimeBeforeSendResponeToPC);

		sendResponeToHost(BOOTLOADER_BROADCAST_PACKET_DONE);
	}
	else
	{
		sendResponeToHost(BOOTLOADER_BROADCAST_PACKET_FAILED);
	}
}

void scanJammingSignal(void)
{
#ifdef RF_USE_nRF24L01
	// Open pipe#0 without Enhanced ShockBurst
	RF24_PIPE_open(RF24_PIPE0, false);
#endif

	uint32_t ui32WaitTime = construct4Byte(&usbBufferHostToDevice[1]);

	GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);

	resetTickCounter();
	// Waiting to see any JAMMING signal from the robots
	while (true)
	{
		if (MCU_RF_IsInterruptPinAsserted())
		{
			MCU_RF_ClearIntFlag();

			GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);

			RfFlushRxFifo();

#ifdef RF_USE_nRF24L01
			RF24_clearIrqFlag(RF24_IRQ_MASK);
#endif

			RfSetRxMode();

			// Set the error byte to zero - indicate Rx asserted!!!
			usbBufferDeviceToHost[0] = BOOTLOADER_SCAN_JAMMING_ASSERT;

			break;
		}

		if(getTickCounterValue() >= ui32WaitTime)
		{
			usbBufferDeviceToHost[0] = BOOTLOADER_SCAN_JAMMING_CLEAR;

			break;
		}
	}

	// Ready to receive data from PC
	setUsbRxState(USB_RX_IDLE);

	// Send the received data to PC
	USB_sendData(0);

#ifdef RF_USE_nRF24L01
	// Open pipe#0 without Enhanced ShockBurst
	RF24_PIPE_open(RF24_PIPE0, true);
#endif

	return;
}


void transmitMessageToRobot(uint8_t* pui8PacketBuffer, bool isAckRequire)
{
	int32_t numberOfTransmittedBytes;

	numberOfTransmittedBytes = construct4Byte(&pui8PacketBuffer[1]);

	if(Network_sendMessage(getRfTxAddress(), &pui8PacketBuffer[5], numberOfTransmittedBytes, isAckRequire))
	{
		sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_DONE);
	}
	else
	{
		sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_FAILED);
	}
}

void receiveDataFromRobot(uint8_t* pui8DataBuffer, bool haveCommand)
{
	uint32_t ui32DataLength = construct4Byte(&pui8DataBuffer[1]);
	uint32_t ui32WaitTime = construct4Byte(&pui8DataBuffer[5]);

	uint32_t i;
	uint32_t ui32MessageSize;
	uint8_t* pui8RxBuffer = 0;
	uint32_t ui32DataPointer;

	if (haveCommand)
	{
		uint32_t comandLength = construct4Byte(&pui8DataBuffer[9]);

		// Transfer the command and the data length to robot, require ack
		if(!Network_sendMessage(getRfTxAddress(), &pui8DataBuffer[13], comandLength, true))
		{
			usbBufferDeviceToHost[USB_MAXIMUM_TRANSMISSION_LENGTH] = RECEIVE_DATA_FROM_ROBOT_ERROR;
			USB_sendData(0);
			return;
		}
	}

	GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);

	resetTickCounter();

	while (true)	// --- received and construct data loop
	{
		if (GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
		{
			GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);

			if (Network_receivedMessage(&pui8RxBuffer, &ui32MessageSize))
			{
				if(ui32MessageSize == ui32DataLength)
					break;
				else
				{
					Network_deleteBuffer(pui8RxBuffer);
					ui32MessageSize = 0;
				}
			}
		}
		// Wait time for receiving data is over?
		if (getTickCounterValue() == ui32WaitTime)
		{ // Signal to the PC we failed to read data from robot
		  // The error signal is at the index 32 since the data read
		  // from robot will be in the range [0:31]
			usbBufferDeviceToHost[USB_MAXIMUM_TRANSMISSION_LENGTH] = RECEIVE_DATA_FROM_ROBOT_ERROR;
			USB_sendData(0);
			Network_deleteBuffer(pui8RxBuffer);
			return;
		}
	}

	ui32DataPointer = 0;

	while(true)	// --- send data to C# software via USB loop
	{
		for(i = 0; i < USB_MAXIMUM_TRANSMISSION_LENGTH && ui32DataPointer < ui32MessageSize; i++)
		{
			usbBufferDeviceToHost[i] = pui8RxBuffer[ui32DataPointer++];
		}

		// Ready to receive data from PC
		setUsbRxState(USB_RX_IDLE);

		// Set the error byte to zero
		usbBufferDeviceToHost[USB_MAXIMUM_TRANSMISSION_LENGTH] = 0;

		// Send the received data to PC
		USB_sendData(0);

		// Is all data transmitted
		if(ui32DataPointer >= ui32MessageSize)
		{
			Network_deleteBuffer(pui8RxBuffer);
			return;
		}

		// Wait for the PC to be ready to receive the next data
		while (getUsbRxState() != USB_RX_DATA_AVAILABLE);

		// Did we receive the right signal?
		if (usbBufferHostToDevice[0] != RECEIVE_DATA_FROM_ROBOT_CONTINUE)
		{
			signalUnhandleError();
			Network_deleteBuffer(pui8RxBuffer);
			return;
		}
	}
}

/* ------------------------------------------------------ Control board Zone */

#endif /* CONTROLBOARD_C_ */
