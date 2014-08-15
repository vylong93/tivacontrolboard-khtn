/*
 * ControlBoard.h
 *
 *  Created on: Jul 31, 2014
 *      Author: MasterE
 */

#ifndef CONTROLBOARD_H_
#define CONTROLBOARD_H_

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_nvic.h"
#include "inc/hw_ints.h"

#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/ssi.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/usb.h"

#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/usblibpriv.h"

#include "usb_swarm_control_structs.h"

#include "libnrf24l01/inc/nRF24L01.h"
#include "libnrf24l01/inc/TM4C123_nRF24L01.h"

extern uint32_t
convertByteToUINT32(uint8_t data[]);

//*****************************************************************************
// Tiva C Series launchpad TM4C123G, RGB LED port definition
//*****************************************************************************
#define LED_PORT_BASE 				GPIO_PORTF_BASE
#define LED_PORT_CLOCK 			        SYSCTL_PERIPH_GPIOF
#define LED_ALL 				(GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define LED_RED   				GPIO_PIN_1
#define LED_BLUE 	 			GPIO_PIN_2
#define LED_GREEN 				GPIO_PIN_3

//*****************************************************************************
// The system tick timer period.
//*****************************************************************************
#define SYSTICKS_PER_SECOND     1000

//*****************************************************************************
// The size of the buffers used by USB Device Driver
//*****************************************************************************
#define USB_BUFFER_SIZE         64

//*****************************************************************************
// The various states that the device can be in during normal operation.
//*****************************************************************************
#define USB_UNCONFIGURED 		0x00
#define USB_RX_IDLE 			0x01
#define USB_TX_IDLE 			0x02
#define USB_TX_SENDING			0x03
#define USB_RX_DATA_AVAILABLE   0x04

//*****************************************************************************
// Host USB Commands
//*****************************************************************************
#define TRANSMIT_DATA_TO_ROBOT 		0x10
#define RECEIVE_DATA_FROM_ROBOT		0x11
#define RECEIVE_DATA_FROM_ROBOT_COMMAND 0x12
#define CONFIGURE_RF				0x13
#define CONFIGURE_SPI				0x14

//*****************************************************************************
// Response to Host USB
//*****************************************************************************
#define CONFIGURE_RF_OK                   	0x12
#define CONFIGURE_SPI_OK                  	0x13
#define TRANSMIT_DATA_TO_ROBOT_DONE 	  	0xAA
#define TRANSMIT_DATA_TO_ROBOT_FAILED   	0xFA
#define RECEIVE_DATA_FROM_ROBOT_ERROR     	0xEE
#define RECEIVE_DATA_FROM_ROBOT_CONTINUE  	0xAE

//*****************************************************************************
// RF transmit maximum packet length
//*****************************************************************************
#define MAX_ALLOWED_DATA_LENGTH 32

inline void
initSystem(void);
inline void
initRfModule(void);
inline void
initUSB(void);

//*****************************************************************************
// !COMMAND from the host
// Configure the SPI module used by the RF board according to the host request
//*****************************************************************************
void
configureSPI(void);

//*****************************************************************************
// !COMMAND from the host
// Configure the RF board according to the host request
//*****************************************************************************
void
configureRF(void);

//*****************************************************************************
// !COMMAND from the host
// Start transmitting data to robots according to the host request
// Maximum transmitted bytes must be smaller than 32 bytes. Otherwise, it will
// call the signalUnhandleError() function.
//*****************************************************************************
void
transmitDataToRobot(void);

//*****************************************************************************
// !COMMAND from the host
// Receive data from other devices and transmit it to the host.
// haveCommand is false when host send RECEIVE_DATA_FROM_ROBOT and
//             is true when host send RECEIVE_DATA_FROM_ROBOT_COMMAND
// If wrong communication command between the host and this device is received,
// it will call the signalUnhandleError() function.
//*****************************************************************************
void
receiveDataFromRobot(bool haveCommand);

//*****************************************************************************
// Read data receive from the robot.
// @param *length: return the length of the received data.
// @param *readData: the pointer point to the buffer to put received data in.
// @param  waiTime: the waiting time to receive a packet
// @return 1: If data is received before the wait time has elapsed. Otherwise, 0.
//*****************************************************************************
bool
readDataFromRobot(uint32_t * length, uint8_t * readData, uint32_t waitTime);

//*****************************************************************************
// This interrupt function is invoked when the host send an event to the device.
// It will call the signalUnhandleError() function if it received a command
// which does not belong to its command table.
//*****************************************************************************
uint32_t
SwarmControlReceiveEventHandler(void *pvCBData, uint32_t ui32Event,
    uint32_t ui32MsgData, void *pvMsgData);

//*****************************************************************************
// This interrupt function is invoked when the device has finished transmitting
// data to the host and an ACK signal is received successfully.
//*****************************************************************************
uint32_t
SwarmControlTransmitEventHandler(void *pvCBData, uint32_t ui32Event,
    uint32_t ui32MsgData, void *pvMsgData);

//*****************************************************************************
// Every data sent to the host through the USB bus must use this function
// for correct operation.
// @param retransMax: The maximum retransmission times to the host
// @return 1: if the data is put on the USB FIFO successfully. Otherwise, 0.
//*****************************************************************************
bool
USB_sendData(uint32_t retransMax);

//*****************************************************************************
// Send a response message to the host
// @param  response: The message to be sent.
//*****************************************************************************
void
sendResponeToHost(uint8_t respone);

//*****************************************************************************
// Signal to the user that we have encounter an unhandled error
//*****************************************************************************
void
signalUnhandleError(void);

//*****************************************************************************
// SysTickIntHandler
//*****************************************************************************
void
SysTickHandler(void);

//*****************************************************************************
// Optional - Tiva C Series launchpad TM4C123G, SW1 Interrupt Handler
//*****************************************************************************
void
LaunchpadButtonIntHandler(void);

#endif /* CONTROLBOARD_H_ */
