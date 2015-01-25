/*
 * ControlBoard_USB.h
 *
 *  Created on: Jan 25, 2015
 *      Author: VyLong
 */

#ifndef CONTROLBOARD_USB_H_
#define CONTROLBOARD_USB_H_

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"

#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/usblibpriv.h"

#include "usb_swarm_control_structs.h"

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
// Configure USB peripheral
//*****************************************************************************
void initUSB(void);

//*****************************************************************************
// This interrupt function is invoked when the host send an event to the device.
// It will call the signalUnhandleError() function if it received a command
// which does not belong to its command table.
//*****************************************************************************
uint32_t SwarmControlReceiveEventHandler(void *pvCBData, uint32_t ui32Event,
    uint32_t ui32MsgData, void *pvMsgData);

//*****************************************************************************
// This interrupt function is invoked when the device has finished transmitting
// data to the host and an ACK signal is received successfully.
//*****************************************************************************
uint32_t SwarmControlTransmitEventHandler(void *pvCBData, uint32_t ui32Event,
    uint32_t ui32MsgData, void *pvMsgData);

//*****************************************************************************
// Every data sent to the host through the USB bus must use this function
// for correct operation.
// @param retransMax: The maximum retransmission times to the host
// @return 1: if the data is put on the USB FIFO successfully. Otherwise, 0.
//*****************************************************************************
bool USB_sendData(uint32_t retransMax);

//*****************************************************************************
// Send a response message to the host
// @param  response: The message to be sent.
//*****************************************************************************
void sendResponeToHost(uint8_t respone);

//*****************************************************************************
// Signal to the user that we have encounter an unhandled error
//*****************************************************************************
void signalUnhandleError(void);


#endif /* CONTROLBOARD_USB_H_ */
