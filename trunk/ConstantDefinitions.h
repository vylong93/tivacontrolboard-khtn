#ifndef _CONSTANT_DEFINITIONS_H
#define _CONSTANT_DEFINITIONS_H

//*****************************************************************************
// Definitions to communicate with the PC software
//*****************************************************************************
#define TRANSMIT_DATA_TO_ROBOT 	0x10
#define RECEIVE_DATA_FROM_ROBOT_WITH_COMMAND 0x11
#define RECEIVE_DATA_NO_COMMAND 0x12
#define CONFIGURE_RF		0x13
#define CONFIGURE_SPI		0x14

//*****************************************************************************
// ACK signals between the control board and the PC software
//*****************************************************************************
#define MAX_ALLOWED_DATA_LENGTH  32

#define CONFIGURE_RF_OK                   0x12
#define CONFIGURE_SPI_OK                  0x13

#define TRANSMIT_DATA_TO_ROBOT_DONE 	  0xAA
#define TRANSMIT_DATA_TO_ROBOT_FAILED     0xFA

#define RECEIVE_DATA_FROM_ROBOT_ERROR     0xEE
#define RECEIVE_DATA_FROM_ROBOT_CONTINUE  0xAE

//*****************************************************************************
// The size of the buffers used by USB Device Driver
//*****************************************************************************
#define USB_BUFFER_SIZE          64

#endif //_CONSTANT_DEFINITIONS_H
