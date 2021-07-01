#ifndef __ADC_H
#define __ADC_H
#include "System_functions.h"

typedef enum 
{
  POWER_OK = 0,
  POWER_LOW,
  POWER_OVER,
  
} POWERSTATE;
/*
typedef enum {
  
  VT22 = 0x20000000,
  VT23 = 0x4000000,
  VT25 = 0x40000000,
  VT26 = 0x8000000,
  VT28 = 0x80000000,
  VT29 = 0x10000000
  
};
*/
#define  DEVIDER_ON        GPIOA->ODR |= GPIO_Pin_5;
#define  DEVIDER_OFF      GPIOA->ODR&= (uint8_t)(~GPIO_Pin_5);

#define ADC_TRESHOLD_UP         2420    //high voltage level(7.4 V)
#define ADC_TRESHOLD_DOWN       1000 //lower voltage level(6.6 V)
#define ADC_CONVERTIONS         3
#define ADC_CHANNEL             ADC_Channel_21
void ADC_int(void);
POWERSTATE ADC_read_pw(uint16_t up, uint16_t down, uint8_t conv);
uint8_t ReadPhotoLine(uint16_t *data);
uint16_t ADC_read_raw(void) ;
#endif