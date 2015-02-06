/*
 * ControlBoard.c
 *
 *  Created on: Jul 31, 2014
 *      Author: MasterE
 */

#ifndef CONTROLBOARD_C_
#define CONTROLBOARD_C_

#include "ControlBoard.h"
#include <stdio.h>
#include <stdlib.h>

extern uint8_t usbBufferHostToDevice[];
extern uint8_t usbBufferDeviceToHost[];

/* ------------------------------------ Control board Zone ----------------------------------------- */

static uint32_t g_ui32TxAddress = RF_DESTINATION_ADDR;

void configureSPI(void)
{
	uint32_t spiProtocol = usbBufferHostToDevice[1];
	uint32_t spiClock = usbBufferHostToDevice[2];
	spiClock = spiClock << 8;
	spiClock |= usbBufferHostToDevice[3];
	spiClock = spiClock << 8;
	spiClock |= usbBufferHostToDevice[4];
	spiClock = spiClock << 8;
	spiClock |= usbBufferHostToDevice[5];
	uint32_t spiDataWidth = usbBufferHostToDevice[6];
	uint32_t spiMode = SSI_MODE_MASTER;

	SSIDisable(RF24_SPI);

	// Configure and enable the SSI port for SPI master mode.
	SSIConfigSetExpClk(RF24_SPI, SysCtlClockGet(), spiProtocol, spiMode,
			spiClock, spiDataWidth);

	SSIEnable(RF24_SPI);

	sendResponeToHost(CONFIGURE_SPI_OK);
}

void configureRF(void)
{
//	RF24_InitTypeDef initRf24;
//	initRf24.CrcBytes = usbBufferHostToDevice[1];
//	initRf24.AddressWidth = usbBufferHostToDevice[2] - 2;
//	initRf24.Channel = usbBufferHostToDevice[3];
//
//	initRf24.CrcState = usbBufferHostToDevice[4];
//	initRf24.Speed = usbBufferHostToDevice[5];
//	initRf24.Power = usbBufferHostToDevice[6];
//	initRf24.LNAGainEnable = usbBufferHostToDevice[7];
//	initRf24.RetransmitCount = RF24_RETRANS_COUNT15;
//	initRf24.RetransmitDelay = RF24_RETRANS_DELAY_250u;
//	initRf24.Features = RF24_FEATURE_EN_DYNAMIC_PAYLOAD
//			| RF24_FEATURE_EN_NO_ACK_COMMAND;
//	initRf24.InterruptEnable = false;
//	RF24_init(&initRf24);
//
//	// Set 2 pipes dynamic payload
//	RF24_PIPE_setPacketSize(RF24_PIPE0, RF24_PACKET_SIZE_DYNAMIC);
//	RF24_PIPE_setPacketSize(RF24_PIPE1, RF24_PACKET_SIZE_DYNAMIC);
//
//	// Open pipe#0, 1 with Enhanced ShockBurst enabled for receiving Auto-ACKs
//	RF24_PIPE_open(RF24_PIPE0, true);
//	RF24_PIPE_open(RF24_PIPE1, true);
//
//	RF24_TX_setAddress(&usbBufferHostToDevice[8]);
//
//	RF24_RX_setAddress(RF24_PIPE0, &usbBufferHostToDevice[11]);
//
//	RF24_RX_activate();

//	uint8_t ui8Channel = usbBufferHostToDevice[1];
//	uint8_t ui8PowerIndex = usbBufferHostToDevice[2];
//	uint8_t ui8AddressWidth = usbBufferHostToDevice[3];

	g_ui32TxAddress = construct4Byte(&usbBufferHostToDevice[4]);

	uint32_t ui32SelfAddress = construct4Byte(&usbBufferHostToDevice[8]);

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


void transmitDataToRobot(void)
{
	int32_t numberOfTransmittedBytes;
	uint32_t delayTimeBeforeSendResponeToPC;

	numberOfTransmittedBytes = usbBufferHostToDevice[1];
	delayTimeBeforeSendResponeToPC = usbBufferHostToDevice[2];

	if(Network_sendMessage(g_ui32TxAddress, &usbBufferHostToDevice[3], numberOfTransmittedBytes, false))
	{
		resetTickCounter();

		while (getTickCounterValue() < delayTimeBeforeSendResponeToPC);

		sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_DONE);

		return;
	}
	else
	{
		sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_FAILED);

		return;
	}
}

void transmitDataToRobotWithACK(void)
{

}

void receiveDataFromRobot(bool haveCommand)
{
	uint32_t ui32DataLength = construct4Byte(&usbBufferHostToDevice[1]);
	uint32_t ui32WaitTime = construct4Byte(&usbBufferHostToDevice[5]);

	uint32_t i;
	uint32_t ui32MessageSize;
	uint8_t* pui8RxBuffer = 0;
	uint32_t ui32DataPointer;

	if (haveCommand)
	{
		uint8_t comandLength = usbBufferHostToDevice[9];

		// Transfer the command and the data length to robot, require ack
		if(!Network_sendMessage(g_ui32TxAddress, &usbBufferHostToDevice[10], comandLength, true))
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

bool readDataFromRobot(uint32_t * length, uint8_t * readData, uint32_t waitTime)
{
	RfSetRxMode();
	MCU_RF_WaitUs(130);

	resetTickCounter();
	while (1)
	{
		if (GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
		{
			GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);

			if (RF24_getIrqFlag(RF24_IRQ_RX))
				break;
		}
		// Wait time for receiving data is over?
		if (getTickCounterValue() == waitTime)
			return 0;
	}

	*length = RF24_RX_getPayloadWidth();
	RF24_RX_getPayloadData(*length, readData);

	// Only clear the IRQ if the RF FIFO is empty
	if (RF24_RX_isEmpty())
		RF24_clearIrqFlag(RF24_IRQ_RX);

	return 1;
}

/* ------------------------------------------------------ Control board Zone */

#endif /* CONTROLBOARD_C_ */
