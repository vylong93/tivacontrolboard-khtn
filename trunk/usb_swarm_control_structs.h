#ifndef _USB_SWARM_CONTROL_STRUCTS_H_
#define _USB_SWARM_CONTROL_STRUCTS_H_

extern uint32_t SwarmControlReceiveEventHandler(void *pvCBData,
                                     uint32_t ui32Event,
                                     uint32_t ui32MsgData,
                                     void *pvMsgData);

extern uint32_t SwarmControlTransmitEventHandler(void *pvCBData,
                                     uint32_t ui32Event,
                                     uint32_t ui32MsgData,
                                     void *pvMsgData);

extern tUSBDHIDDevice g_sHIDSwarmColtrolDevice;

#endif
