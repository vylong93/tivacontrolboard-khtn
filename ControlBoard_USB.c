/*
 * ControlBoard_USB.c
 *
 *  Created on: Jan 25, 2015
 *      Author: VyLong
 */

#include "libcustom\inc\custom_led.h"
#include "ControlBoard_USB.h"


//*****************************************************************************
// The pointer returned by the USBHIDinit function
//*****************************************************************************
void * pvDevice;

//*****************************************************************************
// These variables hold the device configuration information
//*****************************************************************************
volatile bool g_bSuspended = false;
volatile bool g_bConnected = false;
uint32_t g_USBTxState = USB_UNCONFIGURED;
uint32_t g_USBRxState = USB_UNCONFIGURED;

//*****************************************************************************
// This buffer holds the report sent from the host to the device
//*****************************************************************************
uint8_t usbBufferHostToDevice[USB_BUFFER_SIZE];

//*****************************************************************************
// This buffer holds the report to be sent to the host
//*****************************************************************************
uint8_t usbBufferDeviceToHost[USB_BUFFER_SIZE];

//*****************************************************************************
// The buffer to receive data from the RF module and received packet length
//*****************************************************************************
uint8_t g_ui8RxBuffer[32];
uint8_t g_ui8RxLength;

void initUSB(void)
{
	// Configure the required pins for USB operation
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	// Initialize the USB stack for device mode.
	USBStackModeSet(0, eUSBModeForceDevice, 0);

	// Init the usb driver
	pvDevice = USBDHIDInit(0, &g_sHIDSwarmColtrolDevice);

	g_USBRxState = USB_RX_IDLE;
	g_USBTxState = USB_TX_IDLE;
}

uint32_t SwarmControlReceiveEventHandler(void *pvCBData, uint32_t ui32Event,
		uint32_t ui32MsgData, void *pvMsgData)
{

	tUSBDHIDDevice *psHIDDevice;

	// Make sure we did not get a NULL pointer.
	ASSERT(pvCBData);

	psHIDDevice = (tUSBDHIDDevice *) pvCBData;

	switch (ui32Event)
	{
	// The host has connected to us and configured the device.
	case USB_EVENT_CONNECTED:
	{
		GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);
		g_bConnected = true;
		g_bSuspended = false;
		break;
	}

		// The host has disconnected from us.
		// If force device mode is used then VBUS is not monitored by MCU
		// So the USB driver can not generate this event
	case USB_EVENT_DISCONNECTED:
	{
		g_bConnected = false;
		break;
	}

		// This event indicates that the host has suspended the USB bus.
	case USB_EVENT_SUSPEND:
	{
		GPIOPinWrite(LED_PORT_BASE, LED_ALL, 0);
		g_bSuspended = true;
		break;
	}

		// This event signals that the host has resumed signalling on the bus.
	case USB_EVENT_RESUME:
	{
		GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);
		g_bSuspended = false;
		break;
	}

		// This event indicates that the host has sent us an Output or
		// Feature report and that the report is now in the buffer we provided
		// on the previous USBD_HID_EVENT_GET_REPORT_BUFFER callback.
	case USB_EVENT_RX_AVAILABLE:
	{
		// Only receive this packet if the previous one has been used
		if (g_USBRxState == USB_RX_DATA_AVAILABLE)
		{
			GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_GREEN);
			return 0;
		}

		USBDHIDPacketRead(pvCBData, usbBufferHostToDevice, USB_BUFFER_SIZE,
		true);

		g_USBRxState = USB_RX_DATA_AVAILABLE;

		return 0;
	}

		// This event indicates to an application that a report idle timeout has occurred
		// and requests a pointer to the report that must be sent back to the host.
		// The ui32MsgData value contains the requested report ID and pvMsgData contains
		// a pointer that must be written with a pointer to the report data that is to be
		// sent. The callback must return the number of bytes in the report pointed to by
		// *pvMsgData
	case USBD_HID_EVENT_IDLE_TIMEOUT:
	{
		*(uint8_t **) pvMsgData = usbBufferDeviceToHost;
		return (USB_BUFFER_SIZE);
	}

		// Not used since we only have one report type
	case USBD_HID_EVENT_SET_REPORT:
	{
		break;
	}

		// Not used since we use a dedicated interrupt end point.
		// This event indicates that the host has sent a Set_Report request to the device
		// and requests that the device provide a buffer into which the report can be written.
	case USBD_HID_EVENT_GET_REPORT_BUFFER:
	{
		// Are we being asked for a report that is shorter than the storage
		// we have set aside for this?
		if ((uint32_t) pvMsgData <= USB_BUFFER_SIZE)
			return ((uint32_t) usbBufferHostToDevice);
		else
			return (0);
	}

		// Not used since we only have one report type
		// This event indicates that the host is requesting a particular report be returned
		// via endpoint 0, the control endpoint. The ui32MsgValue parameter contains the
		// requested report type in the high byte and report ID in the low byte (as passed
		// in the wValue field of the USB request structure). The pvMsgData parameter
		// contains a pointer which must be written with the address of the first byte of
		// the requested report. The callback must return the size in bytes of the report
		// pointed to by *pvMsgData. The memory returned in response to this event must
		// remain unaltered until USBD_HID_EVENT_REPORT_SENT is sent
	case USBD_HID_EVENT_GET_REPORT:
	{
		*(uint8_t **) pvMsgData = usbBufferDeviceToHost;
		return (USB_BUFFER_SIZE);
	}

		// Not used since we only have one report type
		// This event indicates that a report previously requested via a
		// USBD_HID_EVENT_GET_REPORT has been successfully transmitted to the host.
	case USBD_HID_EVENT_REPORT_SENT:
	{
		break;
	}

		// Not used since we only have one protocol
		// The host is asking us to set either boot or report protocol.
	case USBD_HID_EVENT_SET_PROTOCOL:
	{
		psHIDDevice->ui8Protocol = ui32MsgData;
		break;
	}

		// Not used since we only have one protocol
		// The host is asking us to tell it which protocol we are currently
		// using, boot or request.
	case USBD_HID_EVENT_GET_PROTOCOL:
	{
		return (psHIDDevice->ui8Protocol);
	}

	default:
	{
		//signalUnhandleError();
		while (1)
			GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_ALL);
	}
	}

	return (0);
}

uint32_t SwarmControlTransmitEventHandler(void *pvCBData, uint32_t ui32Event,
		uint32_t ui32MsgData, void *pvMsgData)
{
	switch (ui32Event)
	{
	// We receive this event every time the host acknowledges transmission
	// of a report. It is used here purely as a way of determining whether
	// the host is still talking to us or not.
	case USB_EVENT_TX_COMPLETE:
	{
		// Enter the idle state after finishing sending something.
		g_USBTxState = USB_TX_IDLE;
		break;
	}

	default:
	{
		break;
	}
	}

	return (0);
}

bool USB_sendData(uint32_t retransMax)
{
	uint32_t retransCount = 0;

	// Wait until the usb bus is free to transmit new data
	while (g_USBTxState != USB_TX_IDLE)
		;

	// After the data is received and ack from the host, this variable will be
	// reset to USB_TX_IDLE state by the uint32_t SwarmControlTransmitEventHandler()
	g_USBTxState = USB_TX_SENDING;

	while (retransCount <= retransMax)
	{
		// This function support checking HID state and the index of the data sent.
		if (USBDHIDReportWrite(pvDevice, usbBufferDeviceToHost, USB_BUFFER_SIZE,
		true))
			return 1;

		retransCount++;
	}

	// Reach the maximum number of retransmission
	// Signal to the caller that we can't send data to the host
	return 0;
}

void sendResponeToHost(uint8_t respone)
{
	usbBufferDeviceToHost[0] = respone;
	USB_sendData(0);
}

void signalUnhandleError(void)
{
	while (1)
	{
		GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);
		SysCtlDelay(200000);

		GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_GREEN);
		SysCtlDelay(200000);

		GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);
		SysCtlDelay(200000);
	}
}
