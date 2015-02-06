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

#include "libcustom/inc/custom_clock.h"
#include "libcustom/inc/custom_led.h"
#include "libcustom/inc/custom_button.h"
#include "libcustom/inc/custom_stickTimer.h"
#include "libcustom/inc/custom_uart_debug.h"
#include "libcustom/inc/custom_bluetooth.h"

#ifdef RF_USE_CC2500
#include "libcc2500/inc/TM4C123_CC2500.h"
#include "libcc2500/inc/cc2500.h"
#endif

#ifdef RF_USE_nRF24L01
#include "libnrf24l01/inc/nRF24L01.h"
#include "libnrf24l01/inc/TM4C123_nRF24L01.h"
#endif

#include "libprotocol/inc/network.h"

#include "ControlBoard_USB.h"

//*****************************************************************************
// Default RF addresses
//*****************************************************************************
#define RF_CONTOLBOARD_ADDR		0x00C1AC02
#define RF_DESTINATION_ADDR		NETWORK_BROADCAST_ADDRESS

//*****************************************************************************
// Host USB Commands
//*****************************************************************************
#define BOOTLOADER_BROADCAST_PACKET		0x10
#define BOOTLOADER_SCAN_JAMMING			0x11

#define TRANSMIT_DATA_TO_ROBOT 			0x12
#define	TRANSMIT_DATA_TO_ROBOT_ACK		0x13
#define RECEIVE_DATA_FROM_ROBOT			0x14
#define RECEIVE_DATA_FROM_ROBOT_COMMAND 0x15

#define CONFIGURE_RF				0x16
#define CONFIGURE_SPI				0x17

//*****************************************************************************
// Response to Host USB
//*****************************************************************************
#define	BOOTLOADER_BROADCAST_PACKET_DONE	0x20
#define	BOOTLOADER_BROADCAST_PACKET_FAILED	0x21
#define	BOOTLOADER_SCAN_JAMMING_CLEAR		0x22
#define	BOOTLOADER_SCAN_JAMMING_ASSERT		0x23

#define CONFIGURE_RF_OK                   	0x24
#define CONFIGURE_SPI_OK                  	0x25

#define TRANSMIT_DATA_TO_ROBOT_DONE 	  	0xAA
#define TRANSMIT_DATA_TO_ROBOT_FAILED   	0xFA
#define RECEIVE_DATA_FROM_ROBOT_ERROR     	0xEE
#define RECEIVE_DATA_FROM_ROBOT_CONTINUE  	0xAE

//*****************************************************************************
// RF transmit maximum packet length
//*****************************************************************************
#define MAX_ALLOWED_DATA_LENGTH 28

//*****************************************************************************
// !COMMAND from the host
// Configure the SPI module used by the RF board according to the host request
//*****************************************************************************
void configureSPI(void);

//*****************************************************************************
// !COMMAND from the host
// Configure the RF board according to the host request
//*****************************************************************************
void configureRF(void);

//*****************************************************************************
// !COMMAND from the host
// Start transmitting data to robots according to the host request
// Maximum transmitted bytes must be smaller than 32 bytes. Otherwise, it will
// call the signalUnhandleError() function.
//*****************************************************************************
void transmitDataToRobot(void);

//*****************************************************************************
// !COMMAND from the host
// This function only use in bootloader protocol
// Start transmitting bootloader packet to robots.
//*****************************************************************************
void broadcastBslData(void);

//*****************************************************************************
// !COMMAND from the host
// Receive data from other devices and transmit it to the host.
// haveCommand is false when host send RECEIVE_DATA_FROM_ROBOT and
//             is true when host send RECEIVE_DATA_FROM_ROBOT_COMMAND
// If wrong communication command between the host and this device is received,
// it will call the signalUnhandleError() function.
//*****************************************************************************
void receiveDataFromRobot(bool haveCommand);

//*****************************************************************************
// !COMMAND from the host
// This function only use in bootloader protocol
// Detect the jamming signal and report to the host in a specific period.
//*****************************************************************************
void scanJammingSignal(void);


#endif /* CONTROLBOARD_H_ */
