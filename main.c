#include "ControlBoard.h"

extern volatile bool g_bSuspended;
extern volatile bool g_bConnected;
extern uint32_t g_USBTxState;
extern uint32_t g_USBRxState;
extern uint8_t usbBufferHostToDevice[];
extern uint8_t usbBufferDeviceToHost[];

uint32_t
convertByteToUINT32(uint8_t data[]);

void
main(void)
{
  initSystem();

  while (1)
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
        case CONFIGURE_SPI:
          configureSPI();
          break;

        case CONFIGURE_RF:
          configureRF();
          break;

        case TRANSMIT_DATA_TO_ROBOT:
          transmitDataToRobot();
          break;

        case RECEIVE_DATA_FROM_ROBOT:
          receiveDataFromRobot(false);
          break;

        case RECEIVE_DATA_FROM_ROBOT_COMMAND:
          receiveDataFromRobot(true);
          break;

        default:
          signalUnhandleError();
          break;
      }
      g_USBRxState = USB_RX_IDLE;
      GPIOPinWrite(LED_PORT_BASE, LED_BLUE, 0);
    }
  }
}

//*****************************************************************************
// Convert 4 bytes of a array to uint32
// @param data[]: The converted byte array
// @return : a uint32 value.
//*****************************************************************************
uint32_t
convertByteToUINT32(uint8_t data[])
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
