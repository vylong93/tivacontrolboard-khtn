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
// The pointer returned by the USBHIDinit function
//*****************************************************************************
void * pvDevice;

//*****************************************************************************
// Global system tick counter holds elapsed time since the application started
//*****************************************************************************
volatile uint32_t g_ui32SysTickCount;

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

inline void
initSystem(void)
{
  // Set the clocking to run from the PLL at 50MHz.
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
  SYSCTL_XTAL_16MHZ);

  // Init status led port
  SysCtlPeripheralEnable(LED_PORT_CLOCK);
  GPIOPinTypeGPIOOutput(LED_PORT_BASE, LED_RED | LED_BLUE | LED_GREEN);
  GPIOPinWrite(LED_PORT_BASE, LED_ALL, 0); // turn off all led

  // Set the system tick to fire 1000 times per second.
  SysTickPeriodSet(SysCtlClockGet() / SYSTICKS_PER_SECOND);
  SysTickIntEnable();
  SysTickEnable();

// Testing Only ==============================================
  // unlock the GPIO commit control register to modify PF0 configuration because it may be configured to be a NMI input.
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
  GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_DIR_MODE_IN);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0,
  GPIO_STRENGTH_2MA,
  GPIO_PIN_TYPE_STD_WPU);
  // Configure Interrupt
  GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
  GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
  IntEnable(INT_GPIOF);
  IntPrioritySet(INT_GPIOF, 0x00);
  IntMasterEnable();
// Testing Only ==============================================

  initUSB();

  initRfModule();

  RF24_TX_activate();
}

inline void
initRfModule(void)
{
  RF24_InitTypeDef initRf24;
  initRf24.AddressWidth = RF24_ADRESS_WIDTH_3;
  initRf24.Channel = RF24_CHANNEL_0;
  initRf24.CrcBytes = RF24_CRC_2BYTES;
  initRf24.CrcState = RF24_CRC_EN;
  initRf24.RetransmitCount = RF24_RETRANS_COUNT15;
  initRf24.RetransmitDelay = RF24_RETRANS_DELAY_4000u;
  initRf24.Speed = RF24_SPEED_1MBPS;
  initRf24.Power = RF24_POWER_0DBM;
  initRf24.Features = RF24_FEATURE_EN_DYNAMIC_PAYLOAD
      | RF24_FEATURE_EN_NO_ACK_COMMAND;
  initRf24.InterruptEnable = false;
  initRf24.LNAGainEnable = false;
  RF24_init(&initRf24);

  // Set 2 pipes dynamic payload
  RF24_PIPE_setPacketSize(RF24_PIPE0, RF24_PACKET_SIZE_DYNAMIC);
  RF24_PIPE_setPacketSize(RF24_PIPE1, RF24_PACKET_SIZE_DYNAMIC);

  // Open pipe#0, 1 with Enhanced ShockBurst enabled for receiving Auto-ACKs
  RF24_PIPE_open(RF24_PIPE0, true);
  RF24_PIPE_open(RF24_PIPE1, true);

  uint8_t TxAddrControlBoard[3] =
    { 0xDE, 0xAD, 0xBE };
  uint8_t RxAddrControlBoard[3] =
    { 0x0E, 0xAC, 0xC1 }; // TxRobot1
  uint8_t TxRobot2[3] =
    { 0x02, 0xAC, 0xC1 };

  RF24_TX_setAddress(TxAddrControlBoard);
  RF24_RX_setAddress(RF24_PIPE0, TxRobot2);
  RF24_RX_setAddress(RF24_PIPE1, RxAddrControlBoard);

  RF24_RX_activate();
}

inline void
initUSB(void)
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

void
configureSPI(void)
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
  SSIConfigSetExpClk(RF24_SPI, SysCtlClockGet(), spiProtocol, spiMode, spiClock,
      spiDataWidth);

  SSIEnable(RF24_SPI);

  sendResponeToHost(CONFIGURE_SPI_OK);
}

void
configureRF(void)
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
  initRf24.Features = RF24_FEATURE_EN_DYNAMIC_PAYLOAD
      | RF24_FEATURE_EN_NO_ACK_COMMAND;
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

void
transmitDataToRobot(void)
{
  RF24_TX_activate();

  int32_t numberOfTransmittedBytes;
  uint32_t delayTimeBeforeSendResponeToPC;

  numberOfTransmittedBytes = usbBufferHostToDevice[1];
  delayTimeBeforeSendResponeToPC = usbBufferHostToDevice[2];

  if (numberOfTransmittedBytes > MAX_ALLOWED_DATA_LENGTH)
  {
    signalUnhandleError();
    return;
  }

  uint32_t i;
  for (i = 0; i < numberOfTransmittedBytes; i++)
  {
    usbBufferHostToDevice[i] = usbBufferHostToDevice[3 + i];
  }

  RF24_TX_writePayloadNoAck(numberOfTransmittedBytes, usbBufferHostToDevice);

  RF24_TX_pulseTransmit();

  g_ui32SysTickCount = 0;

  while (g_ui32SysTickCount < SYSTICKS_PER_SECOND)
  {
    if (GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
    {
      if (RF24_getIrqFlag(RF24_IRQ_TX))
      {
        RF24_clearIrqFlag(RF24_IRQ_TX);

        g_ui32SysTickCount = 0;

        while (g_ui32SysTickCount < delayTimeBeforeSendResponeToPC)
          ;

        sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_DONE);

        return;
      }
    }
  }

  RF24_clearIrqFlag(RF24_IRQ_MASK);

  sendResponeToHost(TRANSMIT_DATA_TO_ROBOT_FAILED);
}

void
receiveDataFromRobot(bool haveCommand)
{
  uint32_t dataLength = convertByteToUINT32(&usbBufferHostToDevice[2]);
  uint32_t waitTime = convertByteToUINT32(&usbBufferHostToDevice[6]);

  uint32_t length = 0;

  if (haveCommand)
  {
    // Transfer the command and the data length to robot
    RF24_TX_activate();

    RF24_TX_writePayloadNoAck(5, &usbBufferHostToDevice[1]);

    RF24_TX_pulseTransmit();

    g_ui32SysTickCount = 0;

    while (g_ui32SysTickCount < SYSTICKS_PER_SECOND)
    {
      if (GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
      {
        if (RF24_getIrqFlag(RF24_IRQ_TX))
        {
          RF24_clearIrqFlag(RF24_IRQ_TX);
          break;
        }
      }
    }
  }
  RF24_RX_activate();
 // rfDelayLoop(DELAY_CYCLES_130US);
  GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);

  while (1)
  {
    g_ui32SysTickCount = 0;

    while (1)
    {
      if (GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
      {
        GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);

        if (RF24_getIrqFlag(RF24_IRQ_RX))
        {
          length = RF24_RX_getPayloadWidth();

          RF24_RX_getPayloadData(length, usbBufferDeviceToHost);

          RF24_clearIrqFlag(RF24_IRQ_RX);

          break;
        }
      }
      // Wait time for receiving data is over?
      if (g_ui32SysTickCount == waitTime)
      { // Signal to the PC we failed to read data from robot
        // The error signal is at the index 32 since the data read
        // from robot will be in the range [0:31]
        usbBufferDeviceToHost[32] = RECEIVE_DATA_FROM_ROBOT_ERROR;
        USB_sendData(0);
        return;
      }
    }

    // Ready to receive data from PC
    g_USBRxState = USB_RX_IDLE;

    // Set the error byte to zero
    usbBufferDeviceToHost[32] = 0;

    // Send the received data to PC
    USB_sendData(0);

    // Did we receive all the requested data?
    if (dataLength > length)
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
    while (g_USBRxState != USB_RX_DATA_AVAILABLE)
      ;

    // Did we receive the right signal?
    if (usbBufferHostToDevice[0] != RECEIVE_DATA_FROM_ROBOT_CONTINUE)
    {
      signalUnhandleError();
    }
  }
}

bool
readDataFromRobot(uint32_t * length, uint8_t * readData, uint32_t waitTime)
{
  RF24_RX_activate();
  rfDelayLoop(DELAY_CYCLES_130US);

  g_ui32SysTickCount = 0;
  while (1)
  {
    if (GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
    {
      GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);

      if (RF24_getIrqFlag(RF24_IRQ_RX))
        break;
    }
    // Wait time for receiving data is over?
    if (g_ui32SysTickCount == waitTime)
      return 0;
  }

  *length = RF24_RX_getPayloadWidth();
  RF24_RX_getPayloadData(*length, readData);

  // Only clear the IRQ if the RF FIFO is empty
  if (RF24_RX_isEmpty())
    RF24_clearIrqFlag(RF24_IRQ_RX);

  return 1;
}

uint32_t
SwarmControlReceiveEventHandler(void *pvCBData, uint32_t ui32Event,
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

uint32_t
SwarmControlTransmitEventHandler(void *pvCBData, uint32_t ui32Event,
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

bool
USB_sendData(uint32_t retransMax)
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

void
sendResponeToHost(uint8_t respone)
{
  usbBufferDeviceToHost[0] = respone;
  USB_sendData(0);
}

void
signalUnhandleError(void)
{
  while (1)
  {
    GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);
    SysCtlDelay(2000000);

    GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_GREEN);
    SysCtlDelay(2000000);

    GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);
    SysCtlDelay(2000000);
  }
}

void
SysTickHandler(void)
{
  g_ui32SysTickCount++;
}

void
LaunchpadButtonIntHandler(void)
{
  // Testing only

  RF24_TX_activate();

  SysCtlDelay(533333);  // ~100ms debound

  unsigned char buff = 0xFC; 					// set Distance measure command
  RF24_TX_writePayloadNoAck(1, &buff);
  RF24_TX_pulseTransmit();

  while (GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) != 0)
    ;

  if (RF24_getIrqFlag(RF24_IRQ_TX))
  {
    GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);
  }

  if (RF24_getIrqFlag(RF24_IRQ_MAX_RETRANS))
  {
    GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);
  }

  RF24_clearIrqFlag(RF24_IRQ_MASK);

  SysCtlDelay(533333);  // ~100ms debound

  RF24_RX_activate();

  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
}

#endif /* CONTROLBOARD_C_ */
