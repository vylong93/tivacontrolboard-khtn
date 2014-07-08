#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_gpio.h"
#include "inc/hw_adc.h"
#include "inc/hw_udma.h"
#include "inc/hw_timer.h"
#include "inc/hw_ssi.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/udma.h"
#include "driverlib/timer.h"
#include "driverlib/ssi.h"
#include "driverlib/rom.h"
#include "driverlib/debug.h"

#include "libnrf24l01/inc/TM4C123_nRF24L01.h"
#include "libnrf24l01/inc/nRF24L01.h"
#include "distanceSensing.h"
#include "CustomTivaDrivers.h"

#ifdef gcc
static uint8_t ui8ControlTable[1024] __attribute__ ((aligned (1024)));
#else
static uint8_t ui8ControlTable[1024];
#pragma DATA_ALIGN(ui8ControlTable, 1024)
#endif

void initADC(uint32_t adcClock, uint32_t adcBase, uint32_t adcChannel)
{
  SysCtlPeripheralEnable(adcClock);

  // Hardware Faulterror happens if enable this command.
  // The reason is unknown but it only happens when
  // we step over this function in the debug mode (or let it run consecutively)
  //  ADCSequenceDisable(adcBase, ADC_SEQUENCE_TYPE);

  ADCHardwareOversampleConfigure(adcBase, ADC_AVERAGING_FACTOR);
  ADCSequenceConfigure(adcBase, ADC_SEQUENCE_TYPE, ADC_TRIGGER_TIMER, 0);
  ADCSequenceStepConfigure(adcBase, ADC_SEQUENCE_TYPE, 0, adcChannel | ADC_CTL_IE | ADC_CTL_END);
  ADCDitherEnable(adcBase);

  ADCSequenceEnable(adcBase, ADC_SEQUENCE_TYPE);
  ADCSequenceDMAEnable(adcBase, ADC_SEQUENCE_TYPE);
}

void ADC0IntHandler(void)
{
  uint32_t ui32Status;
  uint32_t ui32Mode;

  ui32Status = ADCIntStatus(ADC0_BASE, ADC_SEQUENCE_TYPE, true);
  ADCIntClear(ADC0_BASE, ui32Status);

  ui32Mode = uDMAChannelModeGet(ADC0_DMA_CHANNEL | UDMA_PRI_SELECT);
  if (ui32Mode == UDMA_MODE_STOP)
  {
    TimerDisable(ADC_TIMER, TIMER_A);
    // Start another request
    uDMAChannelTransferSet(ADC0_DMA_CHANNEL | UDMA_PRI_SELECT, UDMA_MODE_BASIC,
        (void *) (ADC0_BASE + ADC_SEQUENCE_ADDRESS), g_ui16ADC0Result, NUMBER_OF_SAMPLE);
    uDMAChannelEnable(ADC0_DMA_CHANNEL);

  }
}

void ADC1IntHandler(void)
{
  uint32_t ui32Status;
  uint32_t ui32Mode;

  ui32Status = ADCIntStatus(ADC1_BASE, ADC_SEQUENCE_TYPE, true);
  ADCIntClear(ADC1_BASE, ui32Status);

  ui32Mode = uDMAChannelModeGet(ADC1_DMA_CHANNEL | UDMA_PRI_SELECT);
  if (ui32Mode == UDMA_MODE_STOP)
  {
    TimerDisable(ADC_TIMER, TIMER_A);
     //Start another request
    uDMAChannelTransferSet(ADC1_DMA_CHANNEL | UDMA_PRI_SELECT, UDMA_MODE_BASIC,
        (void *) (ADC1_BASE + ADC_SEQUENCE_ADDRESS), g_ui16ADC1Result, NUMBER_OF_SAMPLE);
    uDMAChannelEnable(ADC1_DMA_CHANNEL);

  }
}

void inline DistanceSensing_initModules(void)
{
  // Init ADC
  SysCtlPeripheralEnable(ADC_PORT_CLOCK);
  GPIOPinTypeADC(ADC_PORT, ADC0_IN);
  GPIOPinTypeADC(ADC_PORT, ADC1_IN);
  initADC(SYSCTL_PERIPH_ADC0, ADC0_BASE, ADC0_CHANNEL);
  initADC(SYSCTL_PERIPH_ADC1, ADC1_BASE, ADC1_CHANNEL);

  // uDMA configure
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
  uDMAEnable();
  uDMAControlBaseSet(ui8ControlTable);

  uDMAChannelAssign(DMA_ADC0_CHANNEL);
  uDMAChannelAttributeDisable(ADC0_DMA_CHANNEL, UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
  uDMAChannelControlSet(ADC0_DMA_CHANNEL | UDMA_PRI_SELECT, UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1);
  uDMAChannelTransferSet(ADC0_DMA_CHANNEL | UDMA_PRI_SELECT, UDMA_MODE_BASIC,
      (void *) (ADC0_BASE + ADC_SEQUENCE_ADDRESS), g_ui16ADC0Result, NUMBER_OF_SAMPLE);

  uDMAChannelAssign(DMA_ADC1_CHANNEL);
  uDMAChannelAttributeDisable(ADC1_DMA_CHANNEL, UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
  uDMAChannelControlSet(ADC1_DMA_CHANNEL | UDMA_PRI_SELECT, UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1);
  uDMAChannelTransferSet(ADC1_DMA_CHANNEL | UDMA_PRI_SELECT, UDMA_MODE_BASIC,
      (void *) (ADC1_BASE + ADC_SEQUENCE_ADDRESS), g_ui16ADC1Result, NUMBER_OF_SAMPLE);

  uDMAChannelEnable(ADC0_DMA_CHANNEL);
  uDMAChannelEnable(ADC1_DMA_CHANNEL);

  IntEnable(ADC0_INT);
  IntEnable(ADC1_INT);
  IntEnable(INT_UDMAERR);

  // ADC timer trigger configure
  SysCtlPeripheralEnable(ADC_TIMER_CLOCK);
  TimerDisable(ADC_TIMER, TIMER_A);
  TimerConfigure(ADC_TIMER, TIMER_CFG_PERIODIC);
  TimerLoadSet(ADC_TIMER, TIMER_A, (SysCtlClockGet() / SAMPLE_FREQUENCY));
  TimerControlTrigger(ADC_TIMER, TIMER_A, true);
}

void inline DistanceSensing_start()
{
	TimerEnable(ADC_TIMER, TIMER_A);
}
