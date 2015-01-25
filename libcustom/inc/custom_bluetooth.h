/*
 * custom_bluetooth.h
 *
 *  Created on: Jan 25, 2015
 *      Author: VyLong
 */

#ifndef CUSTOM_BLUETOOTH_H_
#define CUSTOM_BLUETOOTH_H_

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

#define BLUETOOTH_BUFFER_SIZE 34 // RF data + 2 end char (\r\n) 0x0D 0x0A

//*****************************************************************************
// Host Bluetooth Commands
//*****************************************************************************
#define SMART_PHONE_REQUEST_CONFIG	0xAA // 0xF0 is reserved

void initBluetooth(void);

void BluetoothIntHandler(void);

#endif /* CUSTOM_BLUETOOTH_H_ */
