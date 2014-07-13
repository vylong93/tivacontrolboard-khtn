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
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/usblibpriv.h"
#include "driverlib/usb.h"

#include "usb_swarm_control_structs.h"
#include "ConstantDefinitions.h"
//#include "distanceSensing.h"
#include "libnrf24l01/inc/nRF24L01.h"
#include "libnrf24l01/inc/TM4C123_nRF24L01.h"
#include "CustomTivaDrivers.h"

#define LED_PORT GPIO_PORTF_BASE
#define LED_PORT_CLOCK SYSCTL_PERIPH_GPIOF
#define ALL_LED (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

//*****************************************************************************
// The error routine that is called if the driver library encounters an error.
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{

}
#endif

//*****************************************************************************
// These variables hold the device configuration information
//*****************************************************************************
volatile bool g_bSuspended = false;
volatile bool g_bConnected = false;

//*****************************************************************************
// Global system tick counter holds elapsed time since the application started
//*****************************************************************************
volatile uint32_t g_ui32SysTickCount;

//*****************************************************************************
// The system tick timer period.
//*****************************************************************************
#define SYSTICKS_PER_SECOND     1000

//*****************************************************************************
// The various states that the device can be in during normal operation.
//*****************************************************************************
#define USB_UNCONFIGURED 0x00
#define USB_RX_IDLE 0x01
#define USB_TX_IDLE 0x02
#define USB_TX_SENDING 0x03
#define USB_RX_DATA_AVAILABLE   0x04

uint32_t g_USBTxState = USB_UNCONFIGURED;
uint32_t g_USBRxState = USB_UNCONFIGURED;

//*****************************************************************************
// The pointer returned by the USBHIDinit function
//*****************************************************************************
void * pvDevice;

//*****************************************************************************
// This buffer holds the report sent from the host to the device
//*****************************************************************************
uint8_t usbBufferHostToDevice[USB_BUFFER_SIZE];

//*****************************************************************************
// This buffer holds the report to be sent to the host
//*****************************************************************************
uint8_t usbBufferDeviceToHost[USB_BUFFER_SIZE];

//*****************************************************************************
// The variable used to store error information of the uDMA
//*****************************************************************************
uint32_t g_ui32uDMAErrCount = 0;

//*****************************************************************************
// The buffer to receive data from the RF module
//*****************************************************************************
uint8_t RF24_RX_buffer[32];

//*****************************************************************************
// Signal to the user that we have encounter an unhandled error
//*****************************************************************************
void signalUnhandleError()
{
	while(1)
	{
            GPIOPinWrite(LED_PORT, ALL_LED, RED_LED);
            SysCtlDelay(2000000);

            GPIOPinWrite(LED_PORT, ALL_LED, GREEN_LED);
            SysCtlDelay(2000000);

            GPIOPinWrite(LED_PORT, ALL_LED, BLUE_LED);
            SysCtlDelay(2000000);
	}
}

//*****************************************************************************
// SysTickIntHandler
//*****************************************************************************
void SysTickHandler(void)
{
    g_ui32SysTickCount++;
}

//*****************************************************************************
// This interrupt function is invoked when the host send an event to the device.
// It will call the signalUnhandleError() function if it received a command
// which does not belong to its command table.
//*****************************************************************************
uint32_t
SwarmControlReceiveEventHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgData,
                                void *pvMsgData)
{

  tUSBDHIDDevice *psHIDDevice;

  // Make sure we did not get a NULL pointer.
  ASSERT(pvCBData);

  psHIDDevice = (tUSBDHIDDevice *)pvCBData;

    switch (ui32Event)
    {
        // The host has connected to us and configured the device.
        case USB_EVENT_CONNECTED:
        {
          GPIOPinWrite(LED_PORT, ALL_LED, RED_LED);
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
          GPIOPinWrite(LED_PORT, ALL_LED, 0);
          g_bSuspended = true;
          break;
        }

        // This event signals that the host has resumed signalling on the bus.
        case USB_EVENT_RESUME:
        {
          GPIOPinWrite(LED_PORT, ALL_LED, RED_LED);
          g_bSuspended = false;
          break;
        }

        // This event indicates that the host has sent us an Output or
        // Feature report and that the report is now in the buffer we provided
        // on the previous USBD_HID_EVENT_GET_REPORT_BUFFER callback.
        case USB_EVENT_RX_AVAILABLE:
        {
          // Only receive this packet if the previous one has been used
          if(g_USBRxState == USB_RX_DATA_AVAILABLE)
            {
              GPIOPinWrite(LED_PORT, ALL_LED, GREEN_LED);
              return 0;
            }

          USBDHIDPacketRead(pvCBData, usbBufferHostToDevice, USB_BUFFER_SIZE, true);

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
          *(uint8_t **)pvMsgData = usbBufferDeviceToHost;
          return(USB_BUFFER_SIZE);
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
          if((uint32_t)pvMsgData <= USB_BUFFER_SIZE)
            return((uint32_t)usbBufferHostToDevice);
          else
            return(0);
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
          *(uint8_t **)pvMsgData = usbBufferDeviceToHost;
          return(USB_BUFFER_SIZE);
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
          return(psHIDDevice->ui8Protocol);
        }

        default:
        {
          //signalUnhandleError();
        	while(1)
        	GPIOPinWrite(LED_PORT, ALL_LED, ALL_LED);
        }
    }

    return(0);
}

//*****************************************************************************
// This interrupt function is invoked when the device has finished transmitting
// data to the host and an ACK signal is received successfully.
//*****************************************************************************
uint32_t
SwarmControlTransmitEventHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgData,
    void *pvMsgData)
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

  return(0);
}

//*****************************************************************************
// Every data sent to the host through the USB bus must use this function
// for correct operation.
// @param retransMax: The maximum retransmission times to the host
// @return 1: if the data is put on the USB FIFO successfully. Otherwise, 0.
//*****************************************************************************
bool USB_sendData(uint32_t retransMax)
{
  uint32_t retransCount = 0;

  // Wait until the usb bus is free to transmit new data
  while(g_USBTxState != USB_TX_IDLE);

  // After the data is received and ack from the host, this variable will be
  // reset to USB_TX_IDLE state by the uint32_t SwarmControlTransmitEventHandler()
  g_USBTxState = USB_TX_SENDING;

  while(retransCount <= retransMax)
  {
    // This function support checking HID state and the index of the data sent.
    if( USBDHIDReportWrite(pvDevice, usbBufferDeviceToHost, USB_BUFFER_SIZE, true) )
          return 1;

    retransCount++;
  }

  // Reach the maximum number of retransmission
  // Signal to the caller that we can't send data to the host
  return 0;
}

//*****************************************************************************
// Convert 4 bytes of a array to uint32
// @param data[]: The converted byte array
// @return : a uint32 value.
//*****************************************************************************
uint32_t convertByteToUINT32(uint8_t data[])
{
  uint32_t result = data[0];
  result <<= 8;
  result |= data[1];
  result <<= 8;
  result |= data[2];
  result <<= 8;
  result |= data[3];
  return result;
}

//*****************************************************************************
// Send a response message to the host
// @param  response: The message to be sent.
//*****************************************************************************
void sendResponeToHost(uint8_t respone)
{
   usbBufferDeviceToHost[0] = respone;
   USB_sendData(0);
}

//*****************************************************************************
// !COMMAND from the host
// Configure the SPI module used by the RF board according to the host request
//*****************************************************************************
inline void configureSPI()
{
  uint32_t spiProtocol  = usbBufferHostToDevice[1];
  uint32_t spiClock     = usbBufferHostToDevice[2];
  spiClock = spiClock << 8;
  spiClock |= usbBufferHostToDevice[3];
  spiClock = spiClock << 8;
  spiClock |= usbBufferHostToDevice[4];
  spiClock = spiClock << 8;
  spiClock |= usbBufferHostToDevice[5];
  uint32_t spiDataWidth = usbBufferHostToDevice[6];
  uint32_t spiMode      = SSI_MODE_MASTER;

  SSIDisable(RF24_SPI);

  // Configure and enable the SSI port for SPI master mode.
  SSIConfigSetExpClk(RF24_SPI, SysCtlClockGet(), spiProtocol, spiMode, spiClock, spiDataWidth);

  SSIEnable(RF24_SPI);

  sendResponeToHost(CONFIGURE_SPI_OK);
}

//*****************************************************************************
// !COMMAND from the host
// Configure the RF board according to the host request
//*****************************************************************************
inline void configureRF()
{
  RF24_InitTypeDef initRf24;
  initRf24.CrcBytes = usbBufferHostToDevice[1];
  initRf24.AddressWidth = usbBufferHostToDevice[2] - 2;
  initRf24.Channel = usbBufferHostToDevice[3];

  initRf24.CrcState = usbBufferHostToDevice[4];
  initRf24.Speed = usbBufferHostToDevice[5];
  initRf24.Power = usbBufferHostToDevice[6];
  initRf24.LNAGainEnable = usbBufferHostToDevice[7];
  initRf24.RetransmitCount = RF24_RETRANS_DISABLE;
  initRf24.RetransmitDelay = RF24_RETRANS_DELAY_250u;
  initRf24.Features = RF24_FEATURE_EN_DYNAMIC_PAYLOAD | RF24_FEATURE_EN_NO_ACK_COMMAND;
  initRf24.InterruptEnable = false;
  RF24_init(&initRf24);

  // Set payload pipe#0 dynamic
  RF24_PIPE_setPacketSize(RF24_PIPE0, RF24_PACKET_SIZE_DYNAMIC);

  // Open pipe#0 with Enhanced ShockBurst enabled for receiving Auto-ACKs
  RF24_PIPE_open(RF24_PIPE0, true);

  RF24_TX_setAddress(&usbBufferHostToDevice[8]);

  RF24_RX_setAddress(RF24_PIPE0, &usbBufferHostToDevice[11]);

  sendResponeToHost(CONFIGURE_RF_OK);
}

//*****************************************************************************
// Start transmitting data to robots
// @param  *data: The data to be sent.
// @param  *numberOfBytes: The length of the data to be sent.
//*****************************************************************************
bool startTransmitDataToRobot(uint8_t* data, uint8_t numberOfBytes)
{
  RF24_TX_writePayloadNoAck(numberOfBytes, data);
  RF24_TX_pulseTransmit();

  g_ui32SysTickCount = 0;

  while(1)
  {
    if(GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
    {
      if(RF24_getIrqFlag(RF24_IRQ_TX))
      {
          RF24_clearIrqFlag(RF24_IRQ_TX);
          return 1;
      }
    }

    if(g_ui32SysTickCount == SYSTICKS_PER_SECOND)
        return 0;
  }

}

//*****************************************************************************
// !COMMAND from the host
// Start transmitting data to robots according to the host request
// Maximum transmitted bytes must be smaller than 32 bytes. Otherwise, it will
// call the signalUnhandleError() function.
//*****************************************************************************
inline void transmitDataToRobot()
{
  RF24_TX_activate();

  uint32_t numberOfTransmittedBytes;
  uint32_t delayTimeBeforeSendResponeToPC;
  bool success;

  numberOfTransmittedBytes = usbBufferHostToDevice[1];
  delayTimeBeforeSendResponeToPC = usbBufferHostToDevice[2];

  if (numberOfTransmittedBytes > MAX_ALLOWED_DATA_LENGTH)
  {
      signalUnhandleError();
      return;
  }

  uint32_t i;
  for(i = 0; i < numberOfTransmittedBytes; i++)
  {
    usbBufferHostToDevice[i] = usbBufferHostToDevice[3 + i];
  }

  success = startTransmitDataToRobot(usbBufferHostToDevice, numberOfTransmittedBytes);
  if(success == 0)
  {
      sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_FAILED);
      return;
  }

  g_ui32SysTickCount = 0;
  while(g_ui32SysTickCount == delayTimeBeforeSendResponeToPC);

  sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_DONE);
}

//*****************************************************************************
// Read data receive from the robot.
// @param *length: return the length of the received data.
// @param *readData: the pointer point to the buffer to put received data in.
// @param  waiTime: the waiting time to receive a packet
// @return 1: If data is received before the wait time has elapsed. Otherwise, 0.
//*****************************************************************************
bool readDataFromRobot(uint32_t * length, uint8_t * readData, uint32_t waitTime)
{
  g_ui32SysTickCount = 0;
  while(1)
  {
    if(GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
    {
        if(RF24_getIrqFlag(RF24_IRQ_RX))
          break;
    }
    // Wait time for receiving data is over?
    if(g_ui32SysTickCount == waitTime)
      return 0;
  }

  *length = RF24_RX_getPayloadWidth();
  RF24_RX_getPayloadData(*length, readData);

  // Only clear the IRQ if the RF FIFO is empty
  if(RF24_RX_isEmpty())
    RF24_clearIrqFlag(RF24_IRQ_RX);

  return 1;
}

//*****************************************************************************
// !COMMAND from the host
// Receive data from robots and transmit it to the host.
// If wrong communication command between the host and this device is received,
// it will call the signalUnhandleError() function.
//*****************************************************************************
inline void receiveDataFromRobotWithCommand()
{
  uint32_t dataLength = convertByteToUINT32(&usbBufferHostToDevice[2]);
  uint32_t waitTime = convertByteToUINT32(&usbBufferHostToDevice[6]);

  // Transfer the command and the data length to robot
  RF24_TX_activate();
  startTransmitDataToRobot(&usbBufferHostToDevice[1] , 5);

  RF24_RX_activate();
  rfDelayLoop(DELAY_CYCLES_130US);

  uint32_t length = 0;
  while(1)
  {
      if(readDataFromRobot(&length, usbBufferDeviceToHost, waitTime) == 0)
      {   // Signal to the PC we failed to read data from robot
          // The error signal is at the index 32 since the data read
          // from robot will be in the range [0:31]
          usbBufferDeviceToHost[32] = RECEIVE_DATA_FROM_ROBOT_ERROR;
          USB_sendData(0);
        break;
      }

      // Ready to receive data from PC
      g_USBRxState = USB_RX_IDLE;

      // Set the error byte to zero
      usbBufferDeviceToHost[32] = 0;

      // Send the received data to PC
      USB_sendData(0);

      // Did we receive all the requested data?
      if( dataLength > length)
      {
        // No -> re-calculate the number of data need to be received
        dataLength -= length;
      }
      else
      {
        // Yes -> the transmission finished successfully
        break;
      }

      // Wait for the PC to be ready to receive the next data
      while(g_USBRxState != USB_RX_DATA_AVAILABLE);

      // Did we receive the right signal?
      if(usbBufferHostToDevice[0] != RECEIVE_DATA_FROM_ROBOT_CONTINUE)
      {
        signalUnhandleError();
      }
  }
}

//*****************************************************************************
// !COMMAND from the host
// Receive data from other devices and transmit it to the host.
//*****************************************************************************
inline void receiveDataNoCommand()
{
  uint32_t dataLength = convertByteToUINT32(&usbBufferHostToDevice[2]);
  uint32_t waitTime = convertByteToUINT32(&usbBufferHostToDevice[6]);

  RF24_RX_activate();
  rfDelayLoop(DELAY_CYCLES_130US);
  
  uint32_t length = 0;
  while(1)
  {
      if(readDataFromRobot(&length, usbBufferDeviceToHost, waitTime) == 0)
      {   // Signal to the PC we failed to read data from robot
          // The error signal is at the index 32 since the data read
          // from robot will be in the range [0:31]
          usbBufferDeviceToHost[32] = RECEIVE_DATA_FROM_ROBOT_ERROR;
          USB_sendData(0);
        break;
      }

      // Ready to receive data from PC
      g_USBRxState = USB_RX_IDLE;

      // Set the error byte to zero
      usbBufferDeviceToHost[32] = 0;

      // Send the received data to PC
      USB_sendData(0);

      // Did we receive all the requested data?
      if( dataLength > length)
      {
        // No -> re-calculate the number of data need to be received
        dataLength -= length;
      }
      else
      {
        // Yes -> the transmission finished successfully
        break;
      }

      // Wait for the PC to be ready to receive the next data
      while(g_USBRxState != USB_RX_DATA_AVAILABLE);

      // Did we receive the right signal?
      if(usbBufferHostToDevice[0] != RECEIVE_DATA_FROM_ROBOT_CONTINUE)
      {
        signalUnhandleError();
      }
  }
}

//*****************************************************************************
// This function receive command from the host and point to the appropriate
// command handle.
// It will call the signalUnhandleError() function, if a command does not belong
// to its command table.
//*****************************************************************************
void processUsbRequestFromHost(void)
{
    // Check if data was received from the host.
    if(g_USBRxState == USB_RX_DATA_AVAILABLE)
    {
        GPIOPinWrite(LED_PORT, BLUE_LED, BLUE_LED);
        switch(usbBufferHostToDevice[0])
        {
                case CONFIGURE_SPI:
                        configureSPI();
                        break;

                case CONFIGURE_RF:
                        configureRF();
                        break;

                case TRANSMIT_DATA_TO_ROBOT:
                        transmitDataToRobot();
                        break;

                case RECEIVE_DATA_FROM_ROBOT_WITH_COMMAND:
                        receiveDataFromRobotWithCommand();
                        break;

                case RECEIVE_DATA_NO_COMMAND:
                        receiveDataNoCommand();
                        break;

                default:
                        signalUnhandleError();
                        break;
        }
        g_USBRxState = USB_RX_IDLE;
        GPIOPinWrite(LED_PORT, BLUE_LED, 0);
    }
}

//*****************************************************************************
// Init the rf module. Noted that we only support dynamic and noack transmit
// in the control board firmware. However, it can still receive ack packet from
// the robot.
//*****************************************************************************
inline void initRfModule()
{
  RF24_InitTypeDef initRf24;
  initRf24.AddressWidth = RF24_ADRESS_WIDTH_3;
  initRf24.Channel = RF24_CHANNEL_0;
  initRf24.CrcBytes = RF24_CRC_2BYTES;
  initRf24.CrcState = RF24_CRC_EN;
  initRf24.RetransmitCount = RF24_RETRANS_DISABLE;
  initRf24.RetransmitDelay = RF24_RETRANS_DELAY_500u;
  initRf24.Speed = RF24_SPEED_1MBPS;
  initRf24.Power = RF24_POWER_0DBM;
  initRf24.Features = RF24_FEATURE_EN_DYNAMIC_PAYLOAD | RF24_FEATURE_EN_NO_ACK_COMMAND;
  initRf24.InterruptEnable = false;
  initRf24.LNAGainEnable = true;
  RF24_init(&initRf24);

  // Set payload pipe#0 dynamic
  RF24_PIPE_setPacketSize(RF24_PIPE0, RF24_PACKET_SIZE_DYNAMIC);

  // Open pipe#0 with Enhanced ShockBurst enabled for receiving Auto-ACKs
  RF24_PIPE_open(RF24_PIPE0, true);

  uint8_t TxAddrControlBoard[3] = {0xDE, 0xAD, 0xBE};
  uint8_t RxAddrControlBoard[3] = {0x0E, 0xAC, 0xC1};
  
  RF24_TX_setAddress(TxAddrControlBoard);
  RF24_RX_setAddress(RF24_PIPE0, RxAddrControlBoard);
}

//*****************************************************************************
// Init the USB module. Noted that The force device mode is used since  there is
// an error on the VBUS pin of the launchpad preventing the use of this pin.
//*****************************************************************************
inline void initUSB()
{
  // Configure the required pins for USB operation
 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
 GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
 GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

  // Initialize the USB stack for device mode.
 USBStackModeSet(0, eUSBModeForceDevice, 0);

  // Init the usb driver
//  pvDevice = USBDHIDInit(0, &g_sHIDSwarmColtrolDevice);
 pvDevice = USBDHIDInit(0, &g_sHIDSwarmColtrolDevice);

  g_USBRxState =  USB_RX_IDLE;
  g_USBTxState = USB_TX_IDLE;
}

//*****************************************************************************
// Init all the module used in this firmware.
//*****************************************************************************
inline void initSystem()
{
  // Set the clocking to run from the PLL at 50MHz.
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
      SYSCTL_XTAL_16MHZ);

  // Init status led port
  SysCtlPeripheralEnable(LED_PORT_CLOCK);
  GPIOPinTypeGPIOOutput(LED_PORT, RED_LED|BLUE_LED|GREEN_LED);
  GPIOPinWrite(LED_PORT, RED_LED|BLUE_LED|GREEN_LED, 0);

  // Set the system tick to fire 1000 times per second.
  SysTickPeriodSet(SysCtlClockGet() / SYSTICKS_PER_SECOND);
  SysTickIntEnable();
  SysTickEnable();

  initUSB();

  initRfModule();
}

int main(void)
{
  initSystem();

  while(1)
  {
	  if((g_bConnected == false) || (g_bSuspended == true))
	    	continue;

	  processUsbRequestFromHost();
  }
}

//*****************************************************************************
// The interrupt handler for every pin of port E
//*****************************************************************************
void GPIOEIntHandler()
{
}

//*****************************************************************************
// The interrupt handler for the RF board
// All the transmission is handled by polling so we do not need to
// implement this interrupt handle.
//*****************************************************************************
inline void RF24_IntHandler()
{
	// Unexpected interrupt on the RF24_INT_Channel
	signalUnhandleError();
}
