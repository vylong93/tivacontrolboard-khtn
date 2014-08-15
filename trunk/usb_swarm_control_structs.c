#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidkeyb.h"
#include "usb_swarm_control_structs.h"

//****************************************************************************
//
// The languages supported by this device.
//
//****************************************************************************
const uint8_t g_pui8LangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//****************************************************************************
//
// The manufacturer string.
//
//****************************************************************************
const uint8_t g_pui8ManufacturerString[] =
{
    (6 + 1) * 2,
    USB_DTYPE_STRING,
    'F', 0, 'e', 0, 't', 0, 'e', 0, 'l', 0, '.', 0
};

//****************************************************************************
//
// The product string.
//
//****************************************************************************
const uint8_t g_pui8ProductString[] =
{
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'S', 0, 'w', 0, 'a', 0, 'r', 0, 'm', 0, ' ', 0, 'R', 0, 'o', 0, 'b', 0,
    'o', 0, 't', 0, ' ', 0, 'C', 0, 'o', 0, 'l', 0, 't', 0, 'r', 0, 'o', 0,
    'l', 0,
};

//****************************************************************************
//
// The serial number string.
//
//****************************************************************************
const uint8_t g_pui8SeriailNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};

//*****************************************************************************
//
// The interface description string.
//
//*****************************************************************************
const uint8_t g_pui8HIDInterfaceString[] =
{
    (23 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'S', 0, 'w', 0, 'a', 0, 'r', 0,
    'm', 0, ' ', 0, 'C', 0, 'o', 0, 'l', 0, 't', 0, 'r', 0, 'o', 0,
    'l', 0, ' ', 0, 'B', 0, 'o', 0, 'a', 0, 'r', 0, 'd', 0,
};

//*****************************************************************************
//
// The configuration description string.
//
//*****************************************************************************
const uint8_t g_pui8ConfigString[] =
{
    (31 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'S', 0, 'w', 0, 'a', 0, 'r', 0,
    'm', 0, ' ', 0, 'C', 0, 'o', 0, 'l', 0, 't', 0, 'r', 0, 'o', 0,
    'l', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0,
    'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0,
};

//*****************************************************************************
//
// The descriptor string table.
//
//*****************************************************************************
const uint8_t * const g_ppui8StringDescriptors[] =
{
    g_pui8LangDescriptor,
    g_pui8ManufacturerString,
    g_pui8ProductString,
    g_pui8SeriailNumberString,
    g_pui8HIDInterfaceString,
    g_pui8ConfigString,
};

#define NUM_STRING_DESCRIPTORS (sizeof(g_ppui8StringDescriptors) /            \
                                sizeof(uint8_t *))

//*****************************************************************************
// The report descriptor for the swarm robot control board. This report also
// matches with the driver used in the PIC control board.
//*****************************************************************************
#define VENDOR_SPECIFIC 0xFF
static const uint8_t g_pui8ucSwarmControlReportDescriptor[]=
{
	UsagePage(VENDOR_SPECIFIC),
	Usage(USB_HID_POINTER),
	Collection(USB_HID_APPLICATION),
		UsageMinimum(1),
		UsageMaximum(64),
		LogicalMinimum(0),
		LogicalMaximum(255),
		ReportSize(8),
		ReportCount(64),
		Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE | USB_HID_INPUT_ABS),
		UsageMinimum(1),
		UsageMaximum(64),
		Output(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE | USB_HID_INPUT_ABS),
	EndCollection,
};

//*****************************************************************************
// The report descriptor used in the PIC control board.
// Stored here for reference and debuging purposes
//*****************************************************************************
//static const uint8_t g_pui8ucSwarmControlReportDescriptor[]=
//{
//	    0x06, 0x00, 0xFF,       // Usage Page = 0xFF00 (Vendor Defined Page 1)
//	    0x09, 0x01,             // Usage (Vendor Usage 1)
//	    0xA1, 0x01,             // Collection (Application)
//	    0x19, 0x01,             //      Usage Minimum
//	    0x29, 0x40,             //      Usage Maximum 	//64 input usages total (0x01 to 0x40)
//	    0x15, 0x00,             //      Logical Minimum (data bytes in the report may have minimum value = 0x00)
//	    0x26, 0xFF, 0x00, 	  	//      Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
//	    0x75, 0x08,             //      Report Size: 8-bit field size
//	    0x95, 0x40,             //      Report Count: Make sixty-four 8-bit fields (the next time the parser hits an "Input", "Output", or "Feature" item)
//	    0x81, 0x00,             //      Input (Data, Array, Abs): Instantiates input packet fields based on the above report size, count, logical min/max, and usage.
//	    0x19, 0x01,             //      Usage Minimum
//	    0x29, 0x40,             //      Usage Maximum 	//64 output usages total (0x01 to 0x40)
//	    0x91, 0x00,             //      Output (Data, Array, Abs): Instantiates output packet fields.  Uses same report size and count as "Input" fields, since nothing new/different was specified to the parser since the "Input" item.
//	    0xC0                   // End Collection
//};

//*****************************************************************************
//
// The HID class descriptor table. For the swarm control class, we have only
// a single report descriptor.
//
//*****************************************************************************
static const uint8_t * const g_pSwarmControlClassDescriptors[] =
{
	g_pui8ucSwarmControlReportDescriptor
};

//*****************************************************************************
//
// The HID descriptor for the swarm control board. This also matches with
// the driver used in the PIC control board.
//*****************************************************************************
static const tHIDDescriptor g_sSwarmControlHIDDescriptor =
{
	9, // bLength
	USB_HID_DTYPE_HID, // bDescriptorType
	0x0111, // bcdHID (version 1.11 compliant)
	0, // bCountryCode (not localized)
	1, // bNumDescriptors

	{ // array Brace
	    { // struct Brace
	     USB_HID_DTYPE_REPORT,
	     sizeof(g_pui8ucSwarmControlReportDescriptor)
	    }
	},  //A table announcing each of the class-specific descriptors

};

tHIDReportIdle g_psReportIdle[1] = {
	{ 1, 1, 0, 0 }, // Report polled every 4mS (4 * 1).
};

//*****************************************************************************
// Array variables used in Configuration Descriptor
//*****************************************************************************
static const uint8_t g_uiSwarmControlConfigSection[] =
{
    9,
    USB_DTYPE_CONFIGURATION,
    41,
    0x00,
    1,
    1,
    0,
    USB_CONF_ATTR_BUS_PWR,
    50,
};

static const uint8_t g_uiSwarmControlInterfaceSection[] =
{
    9,
    USB_DTYPE_INTERFACE,
    0,
    0,
    2,
    USB_CLASS_HID,
    0,
    0,
    0,
};

static const uint8_t g_uiSwarmControlClassSection[] =
{
    9,
    USB_HID_DTYPE_HID,
    0x11,
    0x01,
    0,
    1,
    USB_HID_DTYPE_REPORT,
    sizeof(g_pui8ucSwarmControlReportDescriptor),
    0x00,
};

#define END_POINT_1_EN 1
static const uint8_t g_uiSwarmControlEndPointInSection[] =
{
    7,
    USB_DTYPE_ENDPOINT,
    END_POINT_1_EN | USB_EP_DESC_IN,  //EndpointAddress
    USB_EP_ATTR_INT,
    64,
    0,
    1,
};

static const uint8_t g_uiSwarmControlEndPointOutSection[] =
{
    7,
    USB_DTYPE_ENDPOINT,
    END_POINT_1_EN | USB_EP_DESC_OUT,  //EndpointAddress
    USB_EP_ATTR_INT,
    64,
    0,
    1,
};

//*****************************************************************************
//
// The HID config section for the swarm control board.
//*****************************************************************************
static const tConfigSection g_sSwarmControlConfigSection = {
		9, g_uiSwarmControlConfigSection };

static const tConfigSection g_sSwarmControlInterfaceSection = {
		9, g_uiSwarmControlInterfaceSection };

static const tConfigSection g_sSwarmControlClassSection = {
		9, g_uiSwarmControlClassSection };

static const tConfigSection g_sSwarmControlEndPointInSection = {
		7, g_uiSwarmControlEndPointInSection };

static const tConfigSection g_sSwarmControlEndPointOutSection = {
		7, g_uiSwarmControlEndPointOutSection };


static const tConfigSection* const g_pSwarmControlConfigSection[] =
{
    &g_sSwarmControlConfigSection,
    &g_sSwarmControlInterfaceSection,
    &g_sSwarmControlClassSection,
    &g_sSwarmControlEndPointInSection,
    &g_sSwarmControlEndPointOutSection,
};

//*****************************************************************************
//
// The HID config descriptor for the swarm control board.
//*****************************************************************************
static const tConfigHeader g_SwarmControlConfigHeader =
{
    //
    //! The number of sections comprising the full descriptor for this
    //! configuration.
    //
    0x05,

    //
    //! A pointer to an array of ui8NumSections section pointers which must
    //! be concatenated to form the configuration descriptor.
    //
    g_pSwarmControlConfigSection,
};

static const tConfigHeader* const g_pSwarmControlConfigHeader =
{
    &g_SwarmControlConfigHeader
};

tUSBDHIDDevice g_sHIDSwarmColtrolDevice =
{
    //
    // The Vendor ID you have been assigned by USB-IF.
    //
    0x04D8,

    //
    // The product ID you have assigned for this device.
    //
    0x003F,

    //
    // The power consumption of your device in milliamps.
    //
    100,

    //
    // The value to be passed to the host in the USB configuration descriptor's
    // bmAttributes field.
    //
    USB_CONF_ATTR_BUS_PWR,

    //
    // The supported boot subclass.
    //
    USB_HID_SCLASS_NONE,

    //
    // The supported protocol.
    //
    USB_HID_PROTOCOL_NONE,

    //
    // The number of Input reports that this device supports.
    //
    64,

    //
    // A pointer to our array of tHIDReportIdle structures. For this device,
    // the array must have 1 element (matching the value of the previous field).
    //
    g_psReportIdle,

    //
    // A pointer to your receive callback event handler.
    //
    SwarmControlReceiveEventHandler,

    //
    // A value that you want passed to the receive callback alongside every
    // event.
    //
    (void *)&g_sHIDSwarmColtrolDevice,

    //
    // A pointer to your transmit callback event handler.
    //
    SwarmControlTransmitEventHandler,

    //
    // A value that you want passed to the transmit callback alongside every
    // event.
    //
    (void *)&g_sHIDSwarmColtrolDevice,

    //
    //! If set to true, this field indicates that the device should use a
    //! dedicated interrupt OUT endpoint to receive reports from the host.  In
    //! this case, reports from the host are passed to the application via the
    //! receive callback using USB_EVENT_RX_AVAILABLE events.  If false,
    //! reports from the host are received via endpoint zero and passed to the
    //! application via USBD_HID_EVENT_REPORT_SENT events.
    //
    true,

    //
    // A pointer to the HID descriptor for the device.
    //
    &g_sSwarmControlHIDDescriptor,

    //
    // A pointer to the array of HID class descriptor pointers for this device.
    // The number of elements in this array and their order must match the
    // information in the HID descriptor provided above.
    //
    g_pSwarmControlClassDescriptors,

    //
    // A pointer to your string table.
    //
    g_ppui8StringDescriptors,

    //
    // The number of entries in your string table. This must equal
    // (1 + (5 + (num HID strings)) * (num languages)).
    //
    NUM_STRING_DESCRIPTORS,

    //
    // ! The configuration descriptor for this HID device.
    //
    &g_pSwarmControlConfigHeader,
};
