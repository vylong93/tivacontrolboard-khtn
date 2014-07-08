#ifndef DISTANCESENSING_H_
#define DISTANCESENSING_H_

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
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/udma.h"
#include "driverlib/timer.h"

//-----------------------Parameters-------------------------//
#define NUMBER_OF_SAMPLE    300
#define SAMPLE_FREQUENCY    100000

#define ADC_PORT_CLOCK          SYSCTL_PERIPH_GPIOE
#define ADC_PORT                GPIO_PORTE_BASE
#define ADC0_IN                 GPIO_PIN_1
#define ADC1_IN                 GPIO_PIN_2
#define ADC0_CHANNEL            ADC_CTL_CH2
#define ADC1_CHANNEL            ADC_CTL_CH1
#define ADC0_INT                INT_ADC0SS3
#define ADC1_INT                INT_ADC1SS3
#define ADC_TIMER_CLOCK         SYSCTL_PERIPH_TIMER0
#define ADC_TIMER               TIMER0_BASE
#define ADC_AVERAGING_FACTOR    8
#define ADC_SEQUENCE_TYPE       3
#define ADC_SEQUENCE_ADDRESS    ADC_O_SSFIFO3
#define ADC0_DMA_CHANNEL        UDMA_CHANNEL_ADC3
#define ADC1_DMA_CHANNEL        UDMA_SEC_CHANNEL_ADC13
#define DMA_ADC0_CHANNEL        UDMA_CH17_ADC0_3
#define DMA_ADC1_CHANNEL        UDMA_CH27_ADC1_3

extern uint16_t g_ui16ADC0Result[NUMBER_OF_SAMPLE];
extern uint16_t g_ui16ADC1Result[NUMBER_OF_SAMPLE];

//-----------------------Functions-------------------------//
// ADC0IntHandler and ADC1IntHandler are handled internally. However
// they must be put into the vector table in the startup file

extern void inline DistanceSensing_initModules(void);

extern void inline DistanceSensing_start(void);

#endif /* DISTANCESENSING_H_ */
